//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_IMESSAGECALLABLE_H
#define ROFROF_IMESSAGECALLABLE_H

namespace RofRof {
    template<bool SSL, bool isServer>
    struct IMessageCallable {
    public:
        virtual void onMessage(uWS::WebSocket<SSL, isServer> *ws, std::string_view message, uWS::OpCode opCode) = 0;
    };
}

#endif //ROFROF_IMESSAGECALLABLE_H
