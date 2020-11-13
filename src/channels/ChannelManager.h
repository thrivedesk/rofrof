//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_CHANNELMANAGER_H
#define ROFROF_CHANNELMANAGER_H

#include <map>
#include <string>
#include <vector>
#include "IChannelManager.h"
#include "PrivateChannel.h"
#include "PresenceChannel.h"
#include "Channel.h"
#include "IChannel.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct ChannelManager : public IChannelManager<SSL, isServer> {
    private:
        RofRof::IChannel<SSL, isServer> *makeChannel(std::string channelName) {
            if (channelName.rfind("private-", 0) == 0) {
                RofRof::Logger::debug("Making private channel");
                RofRof::PrivateChannel<SSL, isServer> *channel = new RofRof::PrivateChannel<SSL, isServer>(channelName);
                return channel;
            }

            if (channelName.rfind("presence-", 0) == 0) {
                RofRof::Logger::debug("Making presence channel");
                RofRof::PresenceChannel<SSL, isServer> *channel = new RofRof::PresenceChannel<SSL, isServer>(channelName);
                return channel;
            }
            RofRof::Logger::debug("Making generic channel");
            RofRof::Channel<SSL, isServer> *channel = new RofRof::Channel<SSL, isServer>(channelName);
            return channel;
        }

    protected:
        std::map<std::string, std::map<std::string, RofRof::IChannel<SSL, isServer> *>> apps;

    public:
        ChannelManager() {
            RofRof::Logger::debug("New channel manager constructed");
        }

        RofRof::IChannel<SSL, isServer> *findOrCreate(std::string appId, std::string channelName) override {
            RofRof::Logger::debug("looking for app");
            auto appsIt = this->apps.find(appId);
            if (appsIt == this->apps.end()) {
                RofRof::Logger::debug("Could not find app");
                auto *channel = this->makeChannel(channelName);
                this->apps[appId][channelName] = channel;
                return channel;
            }

            auto channels = appsIt->second;
            auto channelsIt = channels.find(channelName);
            RofRof::Logger::debug("looking for channel");
            if (channelsIt == channels.end()) {
                RofRof::Logger::debug("Could not find channel");
                auto *channel = this->makeChannel(channelName);
                this->apps[appId][channelName] = channel;
                return channel;
            }

            return channelsIt->second;
        }

        RofRof::IChannel<SSL, isServer> *find(std::string appId, std::string channelName) override {
            auto appsIt = this->apps.find(appId);
            if (appsIt == this->apps.end()) {
                return nullptr;
            }

            auto channels = appsIt->second;
            auto channelsIt = channels.find(channelName);
            if (channelsIt == channels.end()) {
                return nullptr;
            }

            return channelsIt->second;
        }

        std::map<std::string, RofRof::IChannel<SSL, isServer> *> getChannels(std::string appId) override {
            auto appsIt = this->apps.find(appId);
            if (appsIt == this->apps.end()) {
                // throw error later
                std::map<std::string, RofRof::IChannel<SSL, isServer> *> empty;
                return empty;
            }

            return appsIt->second;
        }

        void removeFromAllChannels(uWS::WebSocket<SSL, isServer> *ws) override {
            auto *data = static_cast<RofRof::PerUserData *>(ws->getUserData());

            auto appIt = apps.find(data->app->id);
            if (appIt == apps.end()) {
                return;
            }

            std::map<std::string, RofRof::IChannel<SSL, isServer> *> app = appIt->second;
            for (auto channelIt = app.begin(); channelIt != app.end(); channelIt++) {
                RofRof::IChannel<SSL, isServer> *channel = channelIt->second;
                RofRof::Logger::debug("Unsubscribing from: " , channel->channelName);
                channel->unsubscribe(ws);
            }
            for (auto channelIt = app.cbegin(); channelIt != app.cend();) {
                RofRof::IChannel<SSL, isServer> *channel = channelIt->second;
                if (!channel->hasConnections()) {
                    RofRof::Logger::debug("Erasing channel: " , channel->channelName);
                    app.erase(channelIt++);
                } else {
                    ++channelIt;
                }
            }

            for (auto appIt2 = apps.begin(); appIt2 != apps.end();) {
                std::map<std::string, RofRof::IChannel<SSL, isServer> *> app2 = appIt2->second;
                if (app2.size() == 0) {
                    RofRof::Logger::debug("Erasing app: " , appIt2->first);
                    apps.erase(appIt2++);
                } else {
                    ++appIt2;
                }
            }
        }
    };
}

#endif //ROFROF_CHANNELMANAGER_H
