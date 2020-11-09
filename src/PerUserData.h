//
// Created by rocky on 10/30/20.
//

#include <string>
#include "apps/App.h"

#ifndef ROFROF_PERUSERDATA_H
#define ROFROF_PERUSERDATA_H

namespace RofRof {
    struct  PerUserData {
        std::string socketId;
        RofRof::App* app;
    };
}

#endif //ROFROF_PERUSERDATA_H
