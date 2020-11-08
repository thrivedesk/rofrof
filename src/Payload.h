//
// Created by rocky on 10/30/20.
//

#ifndef ROFROF_PAYLOAD_H
#define ROFROF_PAYLOAD_H

#include "jsoncpp/json/json.h"

namespace RofRof {
    struct Payload {
    public:
        std::string event;
        std::string channel;
        Json::Value message;

        Payload() = default;

        explicit Payload(Json::Value message) {
            this->event = message["event"].asString();
            if(message.isMember("channel")) {
                this->channel = message["channel"].asString();
            }
            this->message = message["data"];
        }
    };
}

#endif //ROFROF_PAYLOAD_H
