//
// Created by rocky on 11/8/20.
//

#ifndef ROFROF_ROFROFCONTROLLER_H
#define ROFROF_ROFROFCONTROLLER_H

#include <iostream>
#include "channels/IChannelManager.h"
#include "absl/strings/match.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct RofRofController {
    private:
        void respond(uWS::HttpResponse<SSL> *res, Json::Value &payload) {
            Json::StreamWriterBuilder builder;

            auto response = Json::writeString(builder, payload);

            res->end(response);
        }

    protected:
        RofRof::IChannelManager<SSL, isServer> *channelManager;
    public:
        explicit RofRofController(RofRof::IChannelManager<SSL, isServer> *channelManager) {
            this->channelManager = channelManager;
        }

        void triggerEvent(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            res->end("Triggers event");
        }

        void fetchChannels(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            Json::Value root;

            std::string appId = (std::string) req->getParameter(0);

            auto info = (std::string) req->getQuery("info");
            auto filter_by_prefix = (std::string) req->getQuery("filter_by_prefix");
            bool withUserCount = absl::StrContains(info, "user_count");

            if (withUserCount && !absl::StartsWith(filter_by_prefix, "presence-")) {
                root["message"] = "Request must be limited to presence channels in order to fetch user_count";
                this->respond(res->writeStatus("400 Bad Request"), root);
                return;
            }

            std::vector<int> counts;

            auto channels = channelManager->getChannels(appId);
            for(auto it = channels.begin(); it != channels.end(); it++) {

            }

            root["user_count"] = withUserCount;

        }

        void fetchChannel(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            res->end("Fetches channel");
        }

        void fetchUsers(uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
            res->end("Fetches users list");
        }
    };
}


#endif //ROFROF_ROFROFCONTROLLER_H
