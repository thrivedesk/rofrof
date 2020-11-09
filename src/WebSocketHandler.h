//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_WEBSOCKETHANDLER_H
#define ROFROF_WEBSOCKETHANDLER_H

#include <sstream>
#include <random>
#include <unordered_set>
#include <jsoncpp/json/json.h>
#include "channels/IChannelManager.h"
#include "messages/IMessageComponent.h"
#include "messages/IMessageCallable.h"
#include "messages/MessageFactory.h"
#include "apps/IAppManager.h"
#include "apps/AppManager.h"

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

    public:
        WebSocketHandler() {
            unsigned int n = 99999999;
            srand(n);

            this->appManager = new RofRof::AppManager<SSL, isServer>();
            this->channelManager = new RofRof::ChannelManager<SSL, isServer>();
            this->messageFactory = new RofRof::MessageFactory<SSL, isServer>();
        }

        App *ensureValidAppId(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            std::string appKey = std::string(req->getParameter(0));
            if (!std::empty(appKey)) {
                App *app = appManager->findByKey(appKey);
                if (app != nullptr) {
                    return app;
                }
            }

            res->writeStatus("401 Unauthorized");
            res->end("Unknown app key " + appKey + " provided.");
            return nullptr;
        }

        void onUpgrade(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req, us_socket_context_t *context) {
            /* You may read from req only here, and COPY whatever you need into your PerSocketData.
             * PerSocketData is valid from .open to .close event, accessed with ws->getUserData().
             * HttpRequest (req) is ONLY valid in this very callback, so any data you will need later
             * has to be COPIED into PerSocketData here. */
            App *app = ensureValidAppId(res, req);
            if (app == nullptr) {
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
        }

        void onOpen(uWS::WebSocket<SSL, isServer> *ws) override {
//            this
//                    ->verifyAppKey(ws)
//                    ->limitConcurrentConnections(ws)
//                    ->generateSocketId(ws)
//                    ->establishConnection(ws);
            auto *data = static_cast<RofRof::PerUserData *>(ws->getUserData());

            std::cout << "New Connection to AppID: " << data->app->id << " With Socket ID: " << data->socketId
                      << std::endl;
            Json::Value root;
            Json::Value rdata;
            rdata["socket_id"] = data->socketId;
            rdata["activity_timeout"] = 30;
            root["event"] = "pusher:connection_established";
            root["data"] = rdata;
            Json::StreamWriterBuilder builder;
            const std::string response = Json::writeString(builder, root);
            ws->send(response, uWS::OpCode::TEXT);
        }

        void onDrain(uWS::WebSocket<SSL, isServer> *ws) {
            std::cout << "Drain" << std::endl;
        }

        void onMessage(uWS::WebSocket<SSL, isServer> *ws, std::string_view message, uWS::OpCode opCode) override {
            std::cout << "Message received: " << message << std::endl;

            JSONCPP_STRING err;
            Json::Value msg;

            Json::CharReaderBuilder builder;
            const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            if (!reader->parse(message.cbegin(), message.cend(), &msg, &err)) {
                std::cout << "error" << std::endl;
                return;
            }

            RofRof::Payload payload(msg);

            auto iMessage = messageFactory->createForMessage(payload, ws, channelManager);
            iMessage->respond();
        }

        void onClose(uWS::WebSocket<SSL, isServer> *ws, int code, std::string_view message) override {
            channelManager->removeFromAllChannels(ws);
            std::cout << "Connection closed" << std::endl;
        }

        void onError(uWS::WebSocket<SSL, isServer> *ws, std::exception exception) override {
            std::cout << "Error occurred" << std::endl;
        }

    protected:
        WebSocketHandler *verifyAppKey(uWS::WebSocket<SSL, isServer> *ws) {
            std::cout << ws->getUserData() << std::endl;
            return this;
        }

        WebSocketHandler *limitConcurrentConnections(uWS::WebSocket<SSL, isServer> *ws) {
            return this;
        }

        WebSocketHandler *generateSocketId(uWS::WebSocket<SSL, isServer> *ws) {
            return this;
        }

        WebSocketHandler *establishConnection(uWS::WebSocket<SSL, isServer> *ws) {
            return this;
        }
    };
}

#endif //ROFROF_WEBSOCKETHANDLER_H
