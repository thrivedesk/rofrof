//
// Created by rocky on 10/30/20.
//

#ifndef ROFROF_CLIENTMESSAGE_H
#define ROFROF_CLIENTMESSAGE_H

#include "iostream"
#include "IMessage.h"
#include "../Payload.h"
#include "../PerUserData.h"
#include "../utils/Logger.h"

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

            if (!data->app->clientMessagesEnabled) {
                RofRof::Logger::debug("Client messaging disabled");
                return;
            }

            RofRof::Logger::debug("Client Message to channel " , payload->channel);

            RofRof::IChannel<SSL, isServer> *channel = channelManager->find(data->app->id, payload->channel);

            if (channel == nullptr) {
                RofRof::Logger::debug("Channel does not exist");
                return;
            }

            Json::Value root;
            root["event"] = payload->event;
            root["channel"] = payload->channel;
            root["data"] = payload->message;

            channel->broadcastToOthers(ws, root);

            RofRof::Logger::debug("Message sent");
        }
    };
}

#endif //ROFROF_CLIENTMESSAGE_H
