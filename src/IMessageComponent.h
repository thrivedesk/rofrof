//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_IMESSAGECOMPONENT_H
#define ROFROF_IMESSAGECOMPONENT_H

#include "WebSocket.h"

namespace RofRof {
    struct IMessageComponent {
    public:
        virtual void onOpen(uWS::WebSocket<true, true> *ws) = 0;

        virtual void onClose(uWS::WebSocket<true, true> *ws) = 0;

        virtual void onError(uWS::WebSocket<true, true> *ws, std::exception exception) = 0;
    };
}

#endif //ROFROF_IMESSAGECOMPONENT_H
