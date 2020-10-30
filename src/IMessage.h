//
// Created by rocky on 10/30/20.
//

#ifndef ROFROF_IMESSAGE_H
#define ROFROF_IMESSAGE_H

namespace RofRof {
    struct IMessage {
    public:
        virtual void respond() = 0;
    };
}

#endif //ROFROF_IMESSAGE_H
