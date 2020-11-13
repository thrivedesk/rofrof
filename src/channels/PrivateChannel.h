//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_PRIVATECHANNEL_H
#define ROFROF_PRIVATECHANNEL_H

#include "IChannel.h"
#include "../utils/Logger.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct PrivateChannel : public IChannel<SSL, isServer> {
    private:
        std::mutex mtx;
    public:
        PrivateChannel(std::string channelName) {
            this->channelName = channelName;
        }

        void subscribe(uWS::WebSocket <SSL, isServer> *ws, RofRof::Payload *payload) override {
            this->verifySignature(ws, payload);
            mtx.lock();
//            std::cout << "Locked PrivateChannel->subscribe" << std::endl;
            this->saveConnection(ws);
            this->connectionCount++;
            mtx.unlock();
//            std::cout << "Unlocked PrivateChannel->subscribe" << std::endl;

            Json::Value root;
            root["event"] = "pusher_internal:subscription_succeeded";
            root["channel"] = this->channelName;
            Json::StreamWriterBuilder builder;
            const std::string response = Json::writeString(builder, root);
            ws->send(response, uWS::OpCode::TEXT);
        }

        void unsubscribe(uWS::WebSocket <SSL, isServer> *ws) override {
            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());

            mtx.lock();
//            std::cout << "Locked PrivateChannel->unsubscribe" << std::endl;
            auto it = this->subscribedConnections.find(data->socketId);
            if (it == this->subscribedConnections.end()) {
                mtx.unlock();
//                std::cout << "Unlocked PrivateChannel->unsubscribe" << std::endl;
                return;
            }
            this->subscribedConnections.erase(it);
            this->connectionCount--;
            mtx.unlock();
//            std::cout << "Unlocked PrivateChannel->unsubscribe" << std::endl;

            RofRof::Logger::debug("Removed " , data->socketId , " from channel " , this->channelName);
        }
    };
}

#endif //ROFROF_PRIVATECHANNEL_H
