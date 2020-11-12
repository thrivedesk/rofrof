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
            std::cout << "Event: " << payload->event << std::endl;
            if (payload->event.rfind("pusher:", 0) == 0) {
                std::cout << "Protocol message" << std::endl;
                return new RofRof::ChannelProtocolMessage<SSL, isServer>(payload, ws, channelManager);
            }
            std::cout << "Client message" << std::endl;
            return new RofRof::ClientMessage<SSL, isServer>(payload, ws, channelManager);
        }
    };
}

#endif //ROFROF_MESSAGEFACTORY_H
