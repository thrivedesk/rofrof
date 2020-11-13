//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_WEBSOCKETHANDLER_H
#define ROFROF_WEBSOCKETHANDLER_H

#include <sstream>
#include <random>
#include <unordered_set>
#include <json/json.h>
#include "channels/IChannelManager.h"
#include "messages/IMessageComponent.h"
#include "messages/IMessageCallable.h"
#include "messages/MessageFactory.h"
#include "apps/IAppManager.h"
#include "apps/AppManager.h"
#include "exceptions/RofRofException.h"
#include "exceptions/InvalidAppKeyException.h"

unsigned long long random10() {
    static std::string digits = "0123456789";
    static std::mt19937 rng(std::random_device{}());

    std::shuffle(digits.begin(), digits.end(), rng);
    return std::stoull(digits);
}

unsigned long long unique_random10() {
    static const std::size_t MAX_NUMBERS = 1'000'000; // adjust as required
    static std::unordered_set<unsigned long long> history;

    if (history.size() == MAX_NUMBERS) throw std::domain_error("limit exceeded");

    while (true) {
        unsigned long long n = random10();
        if (history.insert(n).second) return n;
    }
}

namespace RofRof {
    template<bool SSL, bool isServer>
    struct WebSocketHandler : public IMessageComponent<SSL, isServer>, public IMessageCallable<SSL, isServer> {
        RofRof::IChannelManager<SSL, isServer> *channelManager;
        RofRof::IAppManager<SSL, isServer> *appManager;
        RofRof::MessageFactory<SSL, isServer> *messageFactory;
    private:
        Json::StreamWriterBuilder wBuilder;
        Json::CharReaderBuilder rBuilder;

    public:
        WebSocketHandler() {
            unsigned int n = 99999999;
            srand(n);

            this->appManager = new RofRof::AppManager<SSL, isServer>();
            this->channelManager = new RofRof::ChannelManager<SSL, isServer>();
            this->messageFactory = new RofRof::MessageFactory<SSL, isServer>();
        }

        ~WebSocketHandler() {
            delete appManager;
            delete channelManager;
            delete messageFactory;
        }

        App *ensureValidAppKey(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            std::string appKey = std::string(req->getParameter(0));
            if (!std::empty(appKey)) {
                App *app = appManager->findByKey(appKey);
                if (app != nullptr) {
                    return app;
                }
            }

            throw RofRof::InvalidAppKeyException();
        }

        void onUpgrade(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req, us_socket_context_t *context) {
            try {
                /* You may read from req only here, and COPY whatever you need into your PerSocketData.
                 * PerSocketData is valid from .open to .close event, accessed with ws->getUserData().
                 * HttpRequest (req) is ONLY valid in this very callback, so any data you will need later
                 * has to be COPIED into PerSocketData here. */
                App *app = ensureValidAppKey(res, req);
                if (app->connectionCount >= app->capacity) {
                    res->writeStatus("425 Too Early")->end(R"({"event":"pusher:error", "message":"App capacity exceeded"})");
                    return;
                }

                unsigned long long part1 = unique_random10();
                unsigned long long part2 = unique_random10();

                char buffer[25];
                std::sprintf(buffer, "%llu.%llu", part1, part2);

                std::string socketId = (std::string) buffer;

                /* Immediately upgrading without doing anything "async" before, is simple */
                res->template upgrade<RofRof::PerUserData>({
                                                                   /* We initialize PerSocketData struct here */
                                                                   .socketId = socketId,
                                                                   .app = app
                                                           },
                                                           req->getHeader("sec-websocket-key"),
                                                           req->getHeader("sec-websocket-protocol"),
                                                           req->getHeader("sec-websocket-extensions"),
                                                           context);

                /* If you don't want to upgrade you can instead respond with custom HTTP here,
                 * such as res->writeStatus(...)->writeHeader(...)->end(...); or similar.*/

                /* Performing async upgrade, such as checking with a database is a little more complex;
                 * see UpgradeAsync example instead. */
            } catch (RofRof::RofRofException &e) {
                res->writeStatus(e.status)->end(e.what());
            }
        }

        void onOpen(uWS::WebSocket<SSL, isServer> *ws) override {
            try {
                auto *data = static_cast<RofRof::PerUserData *>(ws->getUserData());
                data->app->connectionCount++;

                RofRof::Logger::debug("New Connection to AppID: " , data->app->id , " With Socket ID: " , data->socketId);
                RofRof::Logger::debug("Total connections: " , std::to_string(data->app->connectionCount));

                Json::Value root;
                Json::Value rdata;
                rdata["socket_id"] = data->socketId;
                rdata["activity_timeout"] = 30;
                root["event"] = "pusher:connection_established";
                root["data"] = rdata;

                const std::string response = Json::writeString(wBuilder, root);
                ws->send(response, uWS::OpCode::TEXT);
            } catch (RofRof::RofRofException &e) {
                ws->send(e.what(), uWS::OpCode::TEXT);
            }
        }

        void onDrain(uWS::WebSocket<SSL, isServer> *ws) {
            RofRof::Logger::debug("Drain");
        }

        void onMessage(uWS::WebSocket<SSL, isServer> *ws, std::string_view message, uWS::OpCode opCode) override {
            try {
                RofRof::Logger::debug("Message received: ", message);

                JSONCPP_STRING err;
                Json::Value msg;

                const std::unique_ptr<Json::CharReader> reader(rBuilder.newCharReader());
                if (!reader->parse(message.cbegin(), message.cend(), &msg, &err)) {
                    RofRof::Logger::error("error parsing message: ", err);
                    return;
                }

                auto *payload = new Payload(msg);

                auto *iMessage = messageFactory->createForMessage(payload, ws, channelManager);
                iMessage->respond();
                delete iMessage;
                delete payload;
            } catch (RofRof::RofRofException &e) {
                std::string response;
                Json::Value root;
                root["event"] = "pusher:error";
                Json::Value data;
                data["code"] = e.code;
                data["message"] = e.status;
                root["data"] = data;

                response = Json::writeString(wBuilder, root);

                ws->send(response, uWS::OpCode::TEXT);
            }
        }

        void onClose(uWS::WebSocket<SSL, isServer> *ws, int code, std::string_view message) override {
            try {
                auto *data = static_cast<PerUserData *>(ws->getUserData());
                data->app->connectionCount--;

                channelManager->removeFromAllChannels(ws);
                RofRof::Logger::debug("Connection closed");
            } catch (RofRof::RofRofException &e) {
                ws->send(e.what(), uWS::OpCode::TEXT);
            }
        }

        void onError(uWS::WebSocket<SSL, isServer> *ws, std::exception exception) override {
            try {
                RofRof::Logger::error("Error occurred:", std::string());
                std::cout << exception.what() << std::endl;
            } catch (RofRof::RofRofException &e) {
                ws->send(e.what(), uWS::OpCode::TEXT);
            }
        }
    };
}

#endif //ROFROF_WEBSOCKETHANDLER_H
