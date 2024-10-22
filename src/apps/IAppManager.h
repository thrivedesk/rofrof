//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_IAPPMANAGER_H
#define ROFROF_IAPPMANAGER_H

#include <vector>
#include "App.h"
#include <json/json.h>

namespace RofRof {
    template<bool SSL, bool isServer>
    struct IAppManager {
        virtual ~IAppManager() = default;
        virtual RofRof::App *findById(std::string appId) = 0;

        virtual RofRof::App *findByKey(std::string appKey) = 0;

        virtual RofRof::App *findBySecret(std::string appSecret) = 0;

        virtual RofRof::App *instantiate(Json::Value &config) = 0;

        virtual std::vector<RofRof::App *> all() = 0;
    };
}

#endif //ROFROF_IAPPMANAGER_H
