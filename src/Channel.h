
#ifndef ROFROF_CHANNEL_CPP
#define ROFROF_CHANNEL_CPP

#include <utility>
#include <vector>
#include <string>
#include "WebSocket.h"
#include "Payload.h"

namespace RofRof {
    struct Channel {
    protected:
        std::string channelName;
        std::vector<std::string> subscribedConnections;

        void verifySignature() {

        }

        void saveConnection(uWS::WebSocket<true, true> *ws) {

        }

    public:
        explicit Channel(std::string channelName) {
            this->channelName = std::move(channelName);
        }

        ~Channel() {
            // TODO: remove all the subscriptions
            std::cout << "Channel gone; poof";
        }

        bool hasConnections() {
            return !subscribedConnections.empty();
        }

        std::vector<std::string> getSubscribedConnections() {
            return subscribedConnections;
        }

        /**
         *
         * @param ws uWS::WebSocket<true, true>
         * @param payload
         */
        void subscribe(uWS::WebSocket<true, true> *ws, Payload payload) {
            this->saveConnection(ws);


            ws->send();
        }

        void unsubscribe() {

        }

        void broadcast() {

        }

        void broadcastToOthers() {

        }

        void broadcastToEveryoneExcept() {

        }
    };
}

#endif // ROFROF_CHANNEL_CPP
