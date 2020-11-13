//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_MESSAGEFACTORY_H
#define ROFROF_MESSAGEFACTORY_H

#include "../Payload.h"
#include "IMessage.h"
#include "../channels/ChannelManager.h"
#include "ChannelProtocolMessage.h"
#include "ClientMessage.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct MessageFactory {
    public:
        RofRof::IMessage* createForMessage(RofRof::Payload *payload, uWS::WebSocket<SSL, isServer> *ws, RofRof::IChannelManager<SSL, isServer> *channelManager) {
            RofRof::Logger::debug("Event: " , payload->event);
            if (payload->event.rfind("pusher:", 0) == 0) {
                RofRof::Logger::debug("Protocol message");
                return new RofRof::ChannelProtocolMessage<SSL, isServer>(payload, ws, channelManager);
            }
            RofRof::Logger::debug("Client message");
            return new RofRof::ClientMessage<SSL, isServer>(payload, ws, channelManager);
        }
    };
}

#endif //ROFROF_MESSAGEFACTORY_H
