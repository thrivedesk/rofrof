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
                std::cout << "Making private channel" << std::endl;
                RofRof::PrivateChannel<SSL, isServer> *channel = new RofRof::PrivateChannel<SSL, isServer>(channelName);
                return channel;
            }

            if (channelName.rfind("presence-", 0) == 0) {
                std::cout << "Making presence channel" << std::endl;
                RofRof::PresenceChannel<SSL, isServer> *channel = new RofRof::PresenceChannel<SSL, isServer>(
                        channelName);
                return channel;
            }
            std::cout << "Making generic channel" << std::endl;
            RofRof::Channel<SSL, isServer> *channel = new RofRof::Channel<SSL, isServer>(channelName);
            return channel;
        }

    protected:
        std::map<std::string, std::map<std::string, RofRof::IChannel<SSL, isServer> *>> apps;

    public:
        ChannelManager() {
            std::cout << "New channel manager constructed" << std::endl;
        }

        ChannelManager(const ChannelManager &) = delete;

        ChannelManager &operator=(const ChannelManager &) = delete;

        ~ChannelManager() = default;

        RofRof::IChannel<SSL, isServer> *findOrCreate(std::string appId, std::string channelName) override {
            std::cout << "looking for app" << std::endl;
            auto appsIt = this->apps.find(appId);
            if (appsIt == this->apps.end()) {
                std::cout << "Could not find app" << std::endl;
                auto *channel = this->makeChannel(channelName);
                this->apps[appId][channelName] = channel;
                return channel;
            }

            auto channels = appsIt->second;
            auto channelsIt = channels.find(channelName);
            std::cout << "looking for channel" << std::endl;
            if (channelsIt == channels.end()) {
                std::cout << "Could not find channel" << std::endl;
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

        unsigned int getConnectionCount(std::string appId) override {
            std::vector<std::string> connections;

            auto channels = this->getChannels(appId);
            for (auto it = channels.begin(); it != channels.end(); it++) {
                auto *channel = it->second;
                std::map<std::string, uWS::WebSocket<SSL, isServer> *> subscriptions = channel->getSubscribedConnections();
                for (auto it2 = subscriptions.begin(); it2 != subscriptions.end(); it2++) {
                    connections.push_back(it2->first);
                }
            }

            std::sort(connections.begin(), connections.end());

            return (unsigned int) (std::unique(connections.begin(), connections.end()) - connections.begin());
        }

        void removeFromAllChannels(uWS::WebSocket<SSL, isServer> *ws) override {
            auto *data = static_cast<RofRof::PerUserData *>(ws->getUserData());

            auto appIt = apps.find(data->appId);
            if (appIt == apps.end()) {
                return;
            }

            std::map<std::string, RofRof::IChannel<SSL, isServer> *> app = appIt->second;
            for (auto channelIt = app.begin(); channelIt != app.end(); channelIt++) {
                RofRof::IChannel<SSL, isServer> *channel = channelIt->second;
                std::cout << "Unsubscribing from: " << channel->channelName << std::endl;
                channel->unsubscribe(ws);
            }
            for (auto channelIt = app.cbegin(); channelIt != app.cend();) {
                RofRof::IChannel<SSL, isServer> *channel = channelIt->second;
                if (!channel->hasConnections()) {
                    std::cout << "Erasing channel: " << channel->channelName << std::endl;
                    app.erase(channelIt++);
                } else {
                    ++channelIt;
                }
            }

            for (auto appIt2 = apps.begin(); appIt2 != apps.end();) {
                std::map<std::string, RofRof::IChannel<SSL, isServer> *> app2 = appIt2->second;
                if (app2.size() == 0) {
                    std::cout << "Erasing app: " << appIt2->first << std::endl;
                    apps.erase(appIt2++);
                } else {
                    ++appIt2;
                }
            }
        }
    };
}

#endif //ROFROF_CHANNELMANAGER_H
