//
// Created by rocky on 10/30/20.
//

#ifndef ROFROF_CLIENTMESSAGE_H
#define ROFROF_CLIENTMESSAGE_H

#include "iostream"
#include "IMessage.h"
#include "../Payload.h"
#include "../PerUserData.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct ClientMessage : public IMessage {
    public:
        Payload *payload;
        uWS::WebSocket<SSL, isServer> *ws;
        RofRof::IChannelManager<SSL, isServer> *channelManager;

        ClientMessage(Payload *payload, uWS::WebSocket<SSL, isServer> *ws, IChannelManager <SSL, isServer> *channelManager) {
            this->payload = payload;
            this->ws = ws;
            this->channelManager = channelManager;
        }

        void respond() override {
            if (payload->event.rfind("client-", 0) != 0) {
                return;
            }

            auto *data = static_cast<RofRof::PerUserData *>(ws->getUserData());
            std::cout << " Client Message to channel " << payload->channel << std::endl;

            RofRof::IChannel<SSL, isServer> *channel = channelManager->find(data->app->id, payload->channel);

            if (channel == nullptr) {
                std::cout << "Channel does not exist" << std::endl;
                return;
            }

            Json::Value root;
            root["event"] = payload->event;
            root["channel"] = payload->channel;
            root["data"] = payload->message;

            channel->broadcastToOthers(ws, root);

            std::cout << "Message sent, connection count: " << channelManager->getConnectionCount(data->app->id)<< std::endl;
        }
    };
}

#endif //ROFROF_CLIENTMESSAGE_H
