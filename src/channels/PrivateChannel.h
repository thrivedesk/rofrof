//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_PRIVATECHANNEL_H
#define ROFROF_PRIVATECHANNEL_H

#include "IChannel.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct PrivateChannel : public IChannel<SSL, isServer> {

    public:
        PrivateChannel(std::string channelName) {
            this->channelName = channelName;
        }

        void subscribe(uWS::WebSocket <SSL, isServer> *ws, RofRof::Payload *payload) override {
            this->verifySignature(ws, payload);
            this->saveConnection(ws);

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
            std::cout << "Removed " << data->socketId << " from channel " << this->channelName << std::endl;
        }
    };
}

#endif //ROFROF_PRIVATECHANNEL_H
