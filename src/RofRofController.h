//
// Created by rocky on 11/8/20.
//

#ifndef ROFROF_ROFROFCONTROLLER_H
#define ROFROF_ROFROFCONTROLLER_H

#include <iostream>
#include "channels/IChannelManager.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct RofRofController {
    private:
        void respond(uWS::HttpResponse<SSL> *res, Json::Value &payload) {
            Json::StreamWriterBuilder builder;

            auto response = Json::writeString(builder, payload);

            res->end(response);
        }

    protected:
        RofRof::IChannelManager<SSL, isServer> *channelManager;
    public:
        explicit RofRofController(RofRof::IChannelManager<SSL, isServer> *channelManager) {
            this->channelManager = channelManager;
        }

        void triggerEvent(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            std::string appId = (std::string) req->getParameter(0);
            std::string channelsJson = (std::string) req->getQuery("channels");
            std::cout << "App ID: " << appId << std::endl;

            /* Allocate automatic, stack, variable as usual */
            std::string buffer;
            /* Move it to storage of lambda */
            res->onData([res, this, appId = std::move(appId), buffer = std::move(buffer)](std::string_view data,
                                                                                          bool last) mutable {
                /* Mutate the captured data */
                buffer.append(data.data(), data.length());

                if (last) {
                    /* When this socket dies (times out) it will RAII release everything */

                    Json::Value payload;
                    JSONCPP_STRING err;
                    auto message = std::string_view(buffer);
                    std::cout << "After convert: " << message << std::endl;
                    Json::CharReaderBuilder builder;
                    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
                    if (!reader->parse(message.cbegin(), message.cend(), &payload, &err)) {
                        std::cout << "error" << std::endl;
                        res->end("Error");
                        return;
                    }
                    buffer = "";
                    message = "";

                    std::cout << "Parsed data: " << payload << std::endl;
                    Json::Value channels = payload["channels"];
                    if (!channels.isArray()) {
                        std::cout << "Channels not an array" << std::endl;
                        res->writeStatus("400 Bad Request")->end("Channel not an array");
                    }

                    std::string dataMsg = payload["data"].asString();
                    std::string eventName = payload["name"].asString();
                    std::string socketId = payload["socket_id"].asString();

                    Json::StreamWriterBuilder wBuilder;

                    for (auto &channel : channels) {
                        std::string channelName = channel.asString();
                        RofRof::IChannel<SSL, isServer> *channelPtr = this->channelManager->find(appId, channelName);
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
            /* Unwind stack, delete buffer, will just skip (heap) destruction since it was moved */
        }

        void fetchChannels(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            Json::Value root;

            std::string appId = (std::string) req->getParameter(0);

            auto info = (std::string) req->getQuery("info");
            auto filter_by_prefix = (std::string) req->getQuery("filter_by_prefix");
            bool withUserCount = info.find("user_count", 0) != std::string::npos;

            if (withUserCount && filter_by_prefix.rfind("presence-", 0) != 0) {
                root["message"] = "Request must be limited to presence channels in order to fetch user_count";
                this->respond(res->writeStatus("400 Bad Request"), root);
                return;
            }

            Json::Value channs;

            auto channels = channelManager->getChannels(appId);
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
        }

        void fetchChannel(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            std::string appId = (std::string) req->getParameter(0);
            std::string channelName = (std::string) req->getParameter(1);

            RofRof::IChannel<SSL, isServer> *channel = channelManager->find(appId, channelName);

            if (channel == nullptr) {
                Json::Value root;
                root["message"] = "The channel name is invalid";
                this->respond(res->writeStatus("404 Not Found"), root);
                return;
            }

            Json::Value root = channel->toArray();
            this->respond(res, root);
        }

        void fetchUsers(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            std::string appId = (std::string) req->getParameter(0);
            std::string channelName = (std::string) req->getParameter(1);

            if (channelName.rfind("presence-", 0) != 0) {
                Json::Value root;
                root["message"] = "Invalid presence channel";
                this->respond(res->writeStatus("400 Bad Request"), root);
                return;
            }

            RofRof::IChannel<SSL, isServer> *channel = channelManager->find(appId, channelName);

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
        }
    };
}


#endif //ROFROF_ROFROFCONTROLLER_H
