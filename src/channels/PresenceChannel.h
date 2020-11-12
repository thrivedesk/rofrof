//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_PRESENCECHANNEL_H
#define ROFROF_PRESENCECHANNEL_H

#include <string>
#include "IChannel.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct PresenceChannel : public IChannel<SSL, isServer> {
        std::map<std::string, Json::Value> users;

    protected:
        std::string getChannelData() {
            Json::Value root;
            Json::Value presence;
            presence["ids"] = this->getUserIds();
            presence["hash"] = this->getHash();
            const unsigned int usersCount = (unsigned int) users.size();
            presence["count"] = usersCount;
            root["presence"] = presence;

            Json::StreamWriterBuilder builder;
            builder.settings_["indentation"] = "";
            builder.settings_["enableYAMLCompatibility"] = false;
            return Json::writeString(builder, root);
        }

        Json::Value getUserIds() {
            Json::Value root;
            int index = 0;
            for (auto it = users.begin(); it != users.end(); it++) {
                root[index++] = it->second["user_id"].asString();
            }

            return root;
        }

        Json::Value getHash() {
            Json::Value root;
            for (auto it = users.begin(); it != users.end(); it++) {
                root[it->second["user_id"].asString()] = it->second["user_info"];
            }
            return root;
        }

        void p_unsubscribe(uWS::WebSocket<SSL, isServer> *ws) {
            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());
            auto it = this->subscribedConnections.find(data->socketId);
            if (it == this->subscribedConnections.end()) {
                return;
            }
            this->subscribedConnections.erase(it);
            std::cout << "Removed " << data->socketId << " from channel " << this->channelName << std::endl;
        }

    public:
        PresenceChannel(std::string channelName) {
            this->channelName = channelName;
        }

        std::map<std::string, Json::Value> getUsers() override {
            return users;
        }

        unsigned int getUsersCount() override {
            return (unsigned int) users.size();
        }

        void subscribe(uWS::WebSocket<SSL, isServer> *ws, RofRof::Payload *payload) override {
            this->verifySignature(ws, payload);
            this->saveConnection(ws);

            std::string_view channel_str_data = payload->message["channel_data"].asCString();

            JSONCPP_STRING err;
            Json::Value channel_data;

            Json::CharReaderBuilder rbuilder;
            const std::unique_ptr<Json::CharReader> reader(rbuilder.newCharReader());
            if (!reader->parse(channel_str_data.cbegin(), channel_str_data.cend(), &channel_data, &err)) {
                std::cout << "parsing error" << std::endl;
                return;
            }

            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());
            this->users[data->socketId] = channel_data;

            Json::Value root;
            root["event"] = "pusher_internal:subscription_succeeded";
            root["channel"] = this->channelName;
            root["data"] = this->getChannelData();

            Json::StreamWriterBuilder builder;
            const std::string response = Json::writeString(builder, root);
            ws->send(response, uWS::OpCode::TEXT);

            Json::Value sroot;
            sroot["event"] = "pusher_internal:member_added";
            sroot["channel"] = this->channelName;
            sroot["data"] = channel_data;

            this->broadcastToOthers(ws, sroot);
        }

        void unsubscribe(uWS::WebSocket<SSL, isServer> *ws) override {
            this->p_unsubscribe(ws);

            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());

            auto it = this->users.find(data->socketId);

            if (it == this->users.end()) {
                return;
            }

            Json::Value root;
            root["event"] = "pusher_internal:member_removed";
            root["channel"] = this->channelName;
            Json::Value rdata;
            rdata["user_id"] = it->second["user_id"].asString();
            root["data"] = rdata;

            this->broadcastToOthers(ws, root);
        }
    };
}
#endif //ROFROF_PRESENCECHANNEL_H
