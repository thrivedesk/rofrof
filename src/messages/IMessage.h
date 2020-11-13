//
// Created by rocky on 10/30/20.
//

#ifndef ROFROF_IMESSAGE_H
#define ROFROF_IMESSAGE_H

namespace RofRof {
    struct IMessage {
    public:
        virtual ~IMessage() = default;
        virtual void respond() {
            RofRof::Logger::error("Not implemented method IMessage::respond()", std::string());
        };
    };
}

#endif //ROFROF_IMESSAGE_H
