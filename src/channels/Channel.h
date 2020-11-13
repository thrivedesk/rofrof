
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
    private:
        std::mutex mtx;
    public:
        Channel() {
            RofRof::Logger::error("Called empty constructor of channel", std::string());
        }

        Channel(std::string channelName) {
            this->channelName = channelName;
        }

        void subscribe(uWS::WebSocket<SSL, isServer> *ws, RofRof::Payload *payload) override {
            mtx.lock();
//            std::cout << "Locked channel->subscribe" << std::endl;
            this->saveConnection(ws);
            this->connectionCount++;
            mtx.unlock();
//            std::cout << "Unlocked channel->subscribe" << std::endl;

            Json::Value root;
            root["event"] = "pusher_internal:subscription_succeeded";
            root["channel"] = this->channelName;
            Json::StreamWriterBuilder builder;
            const std::string response = Json::writeString(builder, root);
            ws->send(response, uWS::OpCode::TEXT);
        }

        void unsubscribe(uWS::WebSocket<SSL, isServer> *ws) override {
            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());

            mtx.lock();
//            std::cout << "Locked channel->unsubscribe" << std::endl;
            auto it = this->subscribedConnections.find(data->socketId);
            if (it == this->subscribedConnections.end()) {
                mtx.unlock();
//                std::cout << "Unlocked channel->unsubscribe" << std::endl;
                return;
            }
            this->subscribedConnections.erase(it);
            mtx.unlock();
//            std::cout << "Unlocked channel->unsubscribe" << std::endl;
            RofRof::Logger::debug("Removed ", data->socketId, " from channel ", this->channelName);

            this->connectionCount--;
        }
    };
}

#endif // ROFROF_CHANNEL_CPP
