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
            std::cout << "Not implemented method IMessage::respond()" << std::endl;
        };
    };
}

#endif //ROFROF_IMESSAGE_H
