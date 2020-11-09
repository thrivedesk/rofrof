
#ifndef ROFROF_ICHANNEL_CPP
#define ROFROF_ICHANNEL_CPP

#include <utility>
#include <vector>
#include <string>
#include "../Payload.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct IChannel {
    protected:
        std::map<std::string, uWS::WebSocket<SSL, isServer> *> subscribedConnections;

        void verifySignature(uWS::WebSocket<SSL, isServer> *ws, RofRof::Payload &payload) {

        }

        void saveConnection(uWS::WebSocket<SSL, isServer> *ws) {
            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());
            this->subscribedConnections[data->socketId] = ws;
            std::cout << "Added " << data->socketId << " to channel " << this->channelName << std::endl;
        }

    public:
        std::string channelName;

        bool hasConnections() {
            std::cout << "Calling hasConnections() in " << this->channelName << std::endl;
            return !subscribedConnections.empty();
        }

        std::map<std::string, uWS::WebSocket<SSL, isServer> *> getSubscribedConnections() {
            return subscribedConnections;
        }

        /**
         *
         * @param ws uWS::WebSocket<true, true>
         * @param payload
         */
        virtual void subscribe(uWS::WebSocket<SSL, isServer> *ws, RofRof::Payload &payload) = 0;

        virtual void unsubscribe(uWS::WebSocket<SSL, isServer> *ws) = 0;

        virtual std::map<std::string, Json::Value> getUsers() {
            // for presence channel
            std::map<std::string, Json::Value> users;
            return users;
        }

        virtual unsigned int getUsersCount() {
            // for presence channel
            return 0;
        }

        void broadcast(std::string response) {
            for (auto it = this->subscribedConnections.begin(); it != this->subscribedConnections.end(); it++) {
                it->second->send(response, uWS::OpCode::TEXT);
            }
        }

        void broadcastToOthers(uWS::WebSocket<SSL, isServer> *ws, const Json::Value &root) {
            Json::StreamWriterBuilder builder;
            const std::string response = Json::writeString(builder, root);

            auto data = static_cast<RofRof::PerUserData *>(ws->getUserData());

            this->broadcastToEveryoneExcept(response, data->socketId);
        }

        void broadcastToEveryoneExcept(std::string response, const std::string &socketId) {
            for (auto it = this->subscribedConnections.begin(); it != this->subscribedConnections.end(); it++) {
                if (it->first != socketId) {
                    it->second->send(response, uWS::OpCode::TEXT);
                }
            }
        }

        Json::Value toArray() {
            auto count = (unsigned int) subscribedConnections.size();

            Json::Value root;
            root["occupied"] = count > 0;
            root["subscription_count"] = count;

            return root;
        }
    };
}

#endif // ROFROF_ICHANNEL_CPP
