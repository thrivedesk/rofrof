//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_IMESSAGECALLABLE_H
#define ROFROF_IMESSAGECALLABLE_H

#include "WebSocket.h"

namespace RofRof {
    struct IMessageCallable {
    public:
        virtual void onMessage(uWS::WebSocket<true, true> *ws) = 0;
    };
}

#endif //ROFROF_IMESSAGECALLABLE_H
