//
// Created by rocky on 10/30/20.
//

#ifndef ROFROF_CLIENTMESSAGE_H
#define ROFROF_CLIENTMESSAGE_H

#include "iostream"
#include "IMessage.h"
#include "Payload.h"
#include "PerUserData.h"
#include "WebSocket.h"

namespace RofRof {
    struct ClientMessage: public IMessage {
    public:
        ClientMessage(Payload payload, auto *ws) {

        }

        void respond() override {

        }
    };
}

#endif //ROFROF_CLIENTMESSAGE_H
