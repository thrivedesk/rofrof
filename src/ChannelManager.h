//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_CHANNELMANAGER_H
#define ROFROF_CHANNELMANAGER_H

#include <map>
#include <vector>
#include "Channel.h"
#include "IChannelManager.h"

namespace RofRof {
    struct ChannelManager : public IChannelManager {
    protected:
        std::map<std::string, std::vector<Channel>> channels;

    public:
        Channel *findOrCreate(std::string appId, std::string channelName) override {

        }

        Channel *find(std::string appId, std::string channelName) override {

        }

        std::vector<Channel> getChannels(std::string appId) override {

        }

        int getConnectionCount(std::string appId) override {
            int count = 0;

            // count all channels

            return count;
        }

        void removeFromAllChannels(int *ws) override {

        }
    };
}

#endif //ROFROF_CHANNELMANAGER_H
