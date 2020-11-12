//
// Created by rocky on 10/31/20.
//

#ifndef ROFROF_CHANNELPROTOCOLMESSAGE_H
#define ROFROF_CHANNELPROTOCOLMESSAGE_H

#include "IMessage.h"
#include "../Payload.h"

namespace RofRof {

    template<bool SSL, bool isServer>
    struct ChannelProtocolMessage : public IMessage {
    private:
        RofRof::Payload *payload;
        RofRof::IChannelManager<SSL, isServer> *channelManager;
        uWS::WebSocket<SSL, isServer> *ws;
    public:
        ChannelProtocolMessage(RofRof::Payload *payload, uWS::WebSocket<SSL, isServer> *ws,RofRof::IChannelManager<SSL, isServer> *channelManager) {
            this->payload = payload;
            this->ws = ws;
            this->channelManager = channelManager;
        }

        void respond() override {
            std::cout << "Event received: " << payload->event << std::endl;
            if (payload->event.rfind("pusher:ping", 0) == 0) {
                this->pong();
            } else if (payload->event.rfind("pusher:subscribe", 0) == 0) {
                this->subscribe();
            } else if (payload->event.rfind("pusher:unsubscribe", 0) == 0) {
                this->unsubscribe();
            } else {
                std::cout << "Unsupported event detected: " << payload->event << std::endl;
            }
        }

    protected:
        void pong() {
            std::cout << "Sending pong" << std::endl;
            this->ws->send(R"({"event":"pusher:pong","data":{}})", uWS::OpCode::TEXT);
        }

        void subscribe() {
            std::cout << "Subscribing" << std::endl;
            auto data = static_cast<RofRof::PerUserData *>(this->ws->getUserData());
            std::cout << "Finding channel" << std::endl;
            auto channel = channelManager->findOrCreate(data->app->id, payload->message["channel"].asString());
            std::cout << "Channel found" << std::endl;
            channel->subscribe(this->ws, this->payload);
            std::cout << "Subscribed" << std::endl;
        }

        void unsubscribe() {
            std::cout << "Unsubscribing" << std::endl;
            auto data = static_cast<RofRof::PerUserData *>(this->ws->getUserData());
            auto channel = channelManager->findOrCreate(data->app->id, payload->message["channel"].asString());
            channel->unsubscribe(this->ws);
        }
    };
}

#endif //ROFROF_CHANNELPROTOCOLMESSAGE_H
