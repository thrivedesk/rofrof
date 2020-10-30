//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_ICHANNELMANAGER_H
#define ROFROF_ICHANNELMANAGER_H

namespace RofRof {
    struct IChannelManager {
    public:
        virtual Channel *findOrCreate(std::string appId, std::string channelName) = 0;

        virtual Channel *find(std::string appId, std::string channelName) = 0;

        virtual std::vector<Channel> getChannels(std::string appId) = 0;

        virtual int getConnectionCount(std::string appId) = 0;

        virtual void removeFromAllChannels(uWS::WebSocket<true, true> *ws) = 0;
    };
}

#endif //ROFROF_ICHANNELMANAGER_H
