//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_IMESSAGECOMPONENT_H
#define ROFROF_IMESSAGECOMPONENT_H

namespace RofRof {
    template<bool SSL, bool isServer>
    struct IMessageComponent {
    public:
        virtual void onOpen(uWS::WebSocket<SSL, isServer> *ws) = 0;

        virtual void onClose(uWS::WebSocket<SSL, isServer> *ws, int code, std::string_view message) = 0;

        virtual void onError(uWS::WebSocket<SSL, isServer> *ws, std::exception exception) = 0;
    };
}

#endif //ROFROF_IMESSAGECOMPONENT_H
