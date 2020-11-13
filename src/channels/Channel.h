
#ifndef ROFROF_CHANNEL_CPP
#define ROFROF_CHANNEL_CPP

#include <utility>
#include <vector>
#include <string>
#include "../Payload.h"
#include "IChannel.h"
#include "../utils/Logger.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct Channel : public RofRof::IChannel<SSL, isServer> {
    public:
        Channel() {
            RofRof::Logger::error("Called empty constructor of channel", std::string());
        }

        Channel(std::string channelName) {
            this->channelName = channelName;
        }

        void subscribe(uWS::WebSocket <SSL, isServer> *ws, RofRof::Payload *payload) override {
            this->saveConnection(ws);

            this->connectionCount++;

            Json::Value root;
            root["event"] = "pusher_internal:subscription_succeeded";
            root["channel"] = this->channelName;
            Json::StreamWriterBuilder builder;
            const std::string response = Json::writeString(builder, root);
            ws->send(response, uWS::OpCode::TEXT);
        }

        void unsubscribe(uWS::WebSocket <SSL, isServer> *ws) override {
            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());
            auto it = this->subscribedConnections.find(data->socketId);
            if (it == this->subscribedConnections.end()) {
                return;
            }

            this->subscribedConnections.erase(it);
            RofRof::Logger::debug("Removed " , data->socketId , " from channel " , this->channelName);

            this->connectionCount--;
        }
    };
}

#endif // ROFROF_CHANNEL_CPP
