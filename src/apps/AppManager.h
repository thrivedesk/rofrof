//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_APPMANAGER_H
#define ROFROF_APPMANAGER_H

#include <vector>
#include "App.h"
#include "IAppManager.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct AppManager : public RofRof::IAppManager<SSL, isServer> {
    private:
        std::vector<RofRof::App *> apps;

    public:
        RofRof::App *findById(std::string appId) override {
            for (auto &app: apps) {
                if (app->id == appId) {
                    return app;
                }
            }

            return nullptr;
        }

        RofRof::App *findByKey(std::string appKey) override {
            for (auto &app: apps) {
                if (app->key == appKey) {
                    return app;
                }
            }

            return nullptr;
        }

        RofRof::App *findBySecret(std::string appSecret) override {
            for (auto &app: apps) {
                if (app->secret == appSecret) {
                    return app;
                }
            }

            return nullptr;
        }

        RofRof::App *instantiate(Json::Value &config) override {
            std::string appId = config["id"].asString();
            std::string appKey = config["key"].asString();
            std::string appSecret = config["secret"].asString();
            std::string appName = config["name"].asString();
            std::string appHost = config["host"].asString();
            std::string appPath = config["path"].asString();
            bool clientMessagesEnabled = config["client_messages_enabled"].asBool();
            unsigned int appCapacity = config["capacity"].asUInt();
            bool statisticsEnabled = config["statistics_enabled"].asBool();

            auto *app = new RofRof::App(appId, appKey, appSecret);

            app
                    ->setName(appName)
                    ->setHost(appHost)
                    ->setPath(appPath)
                    ->setCapacity(appCapacity)
                    ->enableClientMessages(clientMessagesEnabled)
                    ->enableStatistics(statisticsEnabled);

            apps.push_back(app);

            return app;
        }

        std::vector<RofRof::App *> all() override {
            return apps;
        }
    };
}

#endif //ROFROF_APPMANAGER_H
