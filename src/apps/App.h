//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_APP_H
#define ROFROF_APP_H

#include <string>
#include <utility>
#include "../exceptions/InvalidAppIdException.h"
#include "../exceptions/InvalidAppSecretException.h"

namespace RofRof {
    struct App {
    public:
        std::string id;
        std::string key;
        std::string secret;
        std::string name;
        std::string host;
        std::string path;
        unsigned int capacity = 0;
        unsigned int connectionCount = 0;
        bool clientMessagesEnabled = false;
        bool statisticsEnabled = false;

        App(std::string appId, std::string appKey, std::string appSecret) {
            if (std::empty(appKey)) {
                throw RofRof::InvalidAppIdException();
            }

            if (std::empty(appSecret)) {
                throw RofRof::InvalidAppSecretException();
            }

            this->id = std::move(appId);
            this->key = std::move(appKey);
            this->secret = std::move(appSecret);
        }

        App *setName(std::string appName) {
            this->name = std::move(appName);
            return this;
        }

        App *setHost(std::string appHost) {
            this->host = std::move(appHost);
            return this;
        }

        App *setPath(std::string appPath) {
            this->path = std::move(appPath);
            return this;
        }

        App *enableClientMessages(bool enabled = true) {
            this->clientMessagesEnabled = enabled;
            return this;
        }

        App *setCapacity(unsigned int appCapacity) {
            this->capacity = appCapacity;
            return this;
        }

        App *enableStatistics(bool enabled = true) {
            this->statisticsEnabled = enabled;
            return this;
        }
    };
}

#endif //ROFROF_APP_H
