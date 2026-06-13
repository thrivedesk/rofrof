//
// Created by rocky on 11/8/20.
//

#ifndef ROFROF_ROFROFCONTROLLER_H
#define ROFROF_ROFROFCONTROLLER_H

#include <iostream>
#include <algorithm>
#include <cctype>
#include "exceptions/SignatureMismatchException.h"
#include "exceptions/InvalidAppIdException.h"
#include "utils/utils.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct RofRofController {
    protected:
        RofRof::WebSocketHandler<SSL, isServer> *webSocketHandler;

        void respond(uWS::HttpResponse<SSL> *res, Json::Value &payload) {
            Json::StreamWriterBuilder builder;

            auto response = Json::writeString(builder, payload);

            res->end(response);
        }

        App *ensureValidAppId(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            std::string appId = std::string(req->getParameter(0));
            if (!std::empty(appId)) {
                App *app = webSocketHandler->appManager->findById(appId);
                if (app != nullptr) {
                    return app;
                }
            }

            throw RofRof::InvalidAppIdException();
        }

        // Core verifier operating purely on copied strings. uWS::HttpRequest is
        // only valid during the synchronous portion of a handler, so the async
        // POST path must copy these values out of req before calling this.
        std::map<std::string, std::string> verifySignature(const std::string &method, const std::string &url,
                                                            const std::string &queryString,
                                                            const std::string &authSignature,
                                                            App *app, const std::string &content) {
            // ordered map
            std::map<std::string, std::string> reqs;

            // map the query string into params map
            std::vector<std::string> reqPairs = RofRof::Strings::explode(queryString, '&');
            for (const std::string &qPairStr: reqPairs) {
                std::vector<std::string> qPair = RofRof::Strings::explode(qPairStr, '=');
                const std::string key = qPair.at(0);

                if (key == "auth_signature" || key == "body_md5") {
                    continue;
                }

                if (qPair.size() == 2) {
                    reqs[key] = qPair.at(1);
                } else {
                    reqs[key] = "";
                }
            }

            // get content md5 if necessary
            if (!std::empty(content)) {
                reqs["body_md5"] = RofRof::Strings::md5(content);
            }

            std::string upperMethod = method;
            std::transform(upperMethod.begin(), upperMethod.end(), upperMethod.begin(),
                           [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

            // join as string
            std::string signature;
            signature += upperMethod;
            signature += "\n";
            signature += url;
            signature += "\n";
            signature += RofRof::Strings::implode_map('=', '&', reqs);

            std::string calculated_signature = RofRof::Strings::hmac_sha256(app->secret, signature);

            if (!RofRof::Strings::constant_time_equals(calculated_signature, authSignature)) {
                throw RofRof::SignatureMismatchException();
            }

            return reqs;
        }

        // Convenience wrapper for synchronous handlers, where reading from req
        // is still safe.
        std::map<std::string, std::string> ensureValidAppSignature(uWS::HttpRequest *req, App *app, const std::string &content) {
            return verifySignature(std::string(req->getMethod()), std::string(req->getUrl()),
                                   std::string(req->getQuery()), std::string(req->getQuery("auth_signature")),
                                   app, content);
        }

    public:
        explicit RofRofController(RofRof::WebSocketHandler<SSL, isServer> *webSocketHandler) {
            this->webSocketHandler = webSocketHandler;
        }

        void triggerEvent(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            try {
                App *app = this->ensureValidAppId(res, req);

                /* req is ONLY valid during this synchronous callback. The onData
                 * handler below fires later, so copy everything it needs now. */
                std::string method = std::string(req->getMethod());
                std::string url = std::string(req->getUrl());
                std::string query = std::string(req->getQuery());
                std::string authSignature = std::string(req->getQuery("auth_signature"));

                /* Allocate automatic, stack, variable as usual */
                std::string buffer;
                /* Move it to storage of lambda */
                res->onData([res, this, app, method, url, query, authSignature, buffer = std::move(buffer)](std::string_view data, bool last) mutable {
                    /* Mutate the captured data */
                    buffer.append(data.data(), data.length());

                    if (last) {
                        /* When this socket dies (times out) it will RAII release everything */
                        try {
                            verifySignature(method, url, query, authSignature, app, buffer);
                        }
                        catch (RofRof::RofRofException &e) {
                            res->writeStatus(e.status)->end(e.what());
                            return;
                        }

                        Json::Value payload;
                        JSONCPP_STRING err;
                        auto message = std::string_view(buffer);

                        Json::CharReaderBuilder builder;
                        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
                        if (!reader->parse(message.cbegin(), message.cend(), &payload, &err)) {
                            RofRof::Logger::error("error parsing payload", err);
                            res->end("Error");
                            return;
                        }
                        buffer = "";
                        message = "";

                        Json::Value channels = payload["channels"];
                        if (!channels.isArray()) {
                            res->writeStatus("400 Bad Request")->end("Channel not an array");
                            return;
                        }

                        std::string dataMsg = payload["data"].asString();
                        std::string eventName = payload["name"].asString();
                        std::string socketId = payload["socket_id"].asString();

                        Json::StreamWriterBuilder wBuilder;

                        for (auto &channel : channels) {
                            std::string channelName = channel.asString();
                            RofRof::IChannel<SSL, isServer> *channelPtr = this->webSocketHandler->channelManager->find(app->id, channelName);
                            if (channelPtr != nullptr) {

                                Json::Value response;
                                response["channel"] = channelName;
                                response["event"] = eventName;
                                response["data"] = dataMsg;

                                std::string responseData = Json::writeString(wBuilder, response);

                                channelPtr->broadcastToEveryoneExcept(responseData, socketId);
                            }
                        }

                        this->respond(res, payload);
                    }
                });
                res->onAborted([res]() mutable {
                    res->end("Understandable");
                });
            } catch (RofRof::RofRofException &e) {
                res->writeStatus(e.status)->end(e.what());
            }
            /* Unwind stack, delete buffer, will just skip (heap) destruction since it was moved */
        }

        void fetchChannels(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            try {
                App *app = this->ensureValidAppId(res, req);
                ensureValidAppSignature(req, app, "");

                Json::Value root;

                auto info = (std::string) req->getQuery("info");
                auto filter_by_prefix = (std::string) req->getQuery("filter_by_prefix");
                bool withUserCount = info.find("user_count", 0) != std::string::npos;

                if (withUserCount && filter_by_prefix.rfind("presence-", 0) != 0) {
                    root["message"] = "Request must be limited to presence channels in order to fetch user_count";
                    this->respond(res->writeStatus("400 Bad Request"), root);
                    return;
                }

                Json::Value channs;

                auto channels = webSocketHandler->channelManager->getChannels(app->id);
                for (auto it = channels.begin(); it != channels.end(); it++) {
                    RofRof::IChannel<SSL, isServer> *channel = it->second;
                    if (!std::empty(filter_by_prefix) && channel->channelName.rfind(filter_by_prefix, 0) != 0) {
                        continue;
                    }
                    Json::Value channData = channel->toArray();
                    if (withUserCount) {
                        channData["user_count"] = channel->getUsersCount();
                    }
                    channs[it->first] = channData;
                }

                root["channels"] = channs;

                this->respond(res, root);

            } catch (RofRof::RofRofException &e) {
                res->writeStatus(e.status)->end(e.what());
            }
        }

        void fetchChannel(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            try {
                App *app = this->ensureValidAppId(res, req);
                ensureValidAppSignature(req, app, "");

                std::string channelName = (std::string) req->getParameter(1);

                RofRof::IChannel<SSL, isServer> *channel = webSocketHandler->channelManager->find(app->id, channelName);

                if (channel == nullptr) {
                    Json::Value root;
                    root["message"] = "The channel name is invalid";
                    this->respond(res->writeStatus("404 Not Found"), root);
                    return;
                }

                Json::Value root = channel->toArray();
                this->respond(res, root);
            } catch (RofRof::RofRofException &e) {
                res->writeStatus(e.status)->end(e.what());
            }
        }

        void fetchUsers(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            try {
                App *app = this->ensureValidAppId(res, req);
                ensureValidAppSignature(req, app, "");

                std::string channelName = (std::string) req->getParameter(1);

                if (channelName.rfind("presence-", 0) != 0) {
                    Json::Value root;
                    root["message"] = "Invalid presence channel";
                    this->respond(res->writeStatus("400 Bad Request"), root);
                    return;
                }

                RofRof::IChannel<SSL, isServer> *channel = webSocketHandler->channelManager->find(app->id, channelName);

                if (channel == nullptr) {
                    Json::Value root;
                    root["message"] = "Unknown channel";
                    this->respond(res->writeStatus("404 Not Found"), root);
                    return;
                }

                std::map<std::string, Json::Value> usersList = channel->getUsers();
                Json::Value users;

                int i = 0;
                for (auto &it : usersList) {
                    Json::Value user;
                    user["id"] = it.second["user_id"];
                    users[i++] = user;
                }

                Json::Value root;
                root["users"] = users;

                this->respond(res, root);
            } catch (RofRof::RofRofException &e) {
                res->writeStatus(e.status)->end(e.what());
            }
        }
    };
}


#endif //ROFROF_ROFROFCONTROLLER_H
