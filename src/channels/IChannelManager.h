//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_ICHANNELMANAGER_H
#define ROFROF_ICHANNELMANAGER_H

#include "IChannel.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct IChannelManager {
    public:
        virtual ~IChannelManager() = default;
        virtual RofRof::IChannel<SSL, isServer> *findOrCreate(std::string appId, std::string channelName) = 0;

        virtual RofRof::IChannel<SSL, isServer> *find(std::string appId, std::string channelName) = 0;

        virtual std::map<std::string, RofRof::IChannel<SSL, isServer> *> getChannels(std::string appId) = 0;

        virtual void removeFromAllChannels(uWS::WebSocket<SSL, isServer> *ws) = 0;
    };
}

#endif //ROFROF_ICHANNELMANAGER_H
