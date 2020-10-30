//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_WEBSOCKETHANDLER_H
#define ROFROF_WEBSOCKETHANDLER_H

#include "IMessageComponent.h"
#include "IMessageCallable.h"
#include "IChannelManager.h"

namespace RofRof {
    struct WebSocketHandler : public IMessageComponent, public IMessageCallable {

        IChannelManager *channelManager;

    public:
        explicit WebSocketHandler(IChannelManager *channelManager) {
            this->channelManager = channelManager;
        }

        void onOpen(uWS::WebSocket<true, true> *ws) override {
            this
                    ->verifyAppKey(ws)
                    ->limitConcurrentConnections(ws)
                    ->generateSocketId(ws)
                    ->establishConnection(ws);
        }

        void onMessage(uWS::WebSocket<true, true> *ws) override {

        }

        void onClose(uWS::WebSocket<true, true> *ws) override {

        }

        void onError(uWS::WebSocket<true, true> *ws, std::exception exception) override {

        }

    protected:
        WebSocketHandler *verifyAppKey(uWS::WebSocket<true, true> *ws) {
            return this;
        }

        WebSocketHandler *limitConcurrentConnections(uWS::WebSocket<true, true> *ws) {
            return this;
        }

        WebSocketHandler *generateSocketId(uWS::WebSocket<true, true> *ws) {
            return this;
        }

        WebSocketHandler *establishConnection(uWS::WebSocket<true, true> *ws) {
            return this;
        }
    };
}

#endif //ROFROF_WEBSOCKETHANDLER_H
