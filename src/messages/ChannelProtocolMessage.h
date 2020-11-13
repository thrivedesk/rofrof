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
            RofRof::Logger::debug("Event received: " , payload->event);
            if (payload->event.rfind("pusher:ping", 0) == 0) {
                this->pong();
            } else if (payload->event.rfind("pusher:subscribe", 0) == 0) {
                this->subscribe();
            } else if (payload->event.rfind("pusher:unsubscribe", 0) == 0) {
                this->unsubscribe();
            } else {
                RofRof::Logger::debug("Unsupported event detected: " , payload->event);
            }
        }

    protected:
        void pong() {
            RofRof::Logger::debug("Sending pong");
            this->ws->send(R"({"event":"pusher:pong","data":{}})", uWS::OpCode::TEXT);
        }

        void subscribe() {
            RofRof::Logger::debug("Subscribing");
            auto data = static_cast<RofRof::PerUserData *>(this->ws->getUserData());
            RofRof::Logger::debug("Finding channel");
            auto channel = channelManager->findOrCreate(data->app->id, payload->message["channel"].asString());
            RofRof::Logger::debug("Channel found");
            channel->subscribe(this->ws, this->payload);
            RofRof::Logger::debug("Subscribed");
        }

        void unsubscribe() {
            RofRof::Logger::debug("Unsubscribing");
            auto data = static_cast<RofRof::PerUserData *>(this->ws->getUserData());
            auto channel = channelManager->findOrCreate(data->app->id, payload->message["channel"].asString());
            channel->unsubscribe(this->ws);
        }
    };
}

#endif //ROFROF_CHANNELPROTOCOLMESSAGE_H
