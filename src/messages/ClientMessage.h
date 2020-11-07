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
        Payload payload;
        uWS::WebSocket<SSL, isServer> *ws;
        RofRof::IChannelManager<SSL, isServer> *channelManager;

        ClientMessage(Payload &payload, uWS::WebSocket<SSL, isServer> *ws,IChannelManager <SSL, isServer> *channelManager) {
            this->payload = payload;
            this->ws = ws;
            this->channelManager = channelManager;
        }

        ClientMessage(const ClientMessage &) = delete;

        ClientMessage &operator=(const ClientMessage &) = delete;

        ~ClientMessage() = default;


        void respond() override {
            if (payload.event.rfind("client-", 0) == 0) {
                return;
            }
            std::cout << " Client Message " << std::endl;
//            PerUserData userData = this->ws->getUserData();

//            if(this->ws->getUserData()) // check if client message enabled
//            IChannel channel = this->channelManager->find(userData->app->)
        }
    };
}

#endif //ROFROF_CLIENTMESSAGE_H
