#include <thread>
#include <iostream>
#include "App.h"
#include "HttpParser.h"
#include "HttpResponse.h"
#include "PerUserData.h"
#include "WebSocketHandler.h"
#include "RofRofController.h"
#include "apps/ConfigFileReader.h"

int main() {
    const bool SSL = false;
    const bool isServer = true;
    auto *websocketHandler = new RofRof::WebSocketHandler<SSL, isServer>();
    auto *controller = new RofRof::RofRofController<SSL, isServer>(websocketHandler);

    auto *configReader = new RofRof::ConfigFileReader<SSL, isServer>();
    configReader->read("apps.json")->make(websocketHandler->appManager);

    uWS::App()
            .get("/*", [](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                res->end("The fastest WebSocket server in the world!!!");
            })
            .post("/apps/:appId/events", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                try {
                    controller->triggerEvent(res, req);
                } catch (RofRof::RofRofException &e) {
                    res->writeStatus(e.status)->end(e.what());
                }
            })
            .get("/apps/:appId/channels", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                try {
                    controller->fetchChannels(res, req);
                } catch (RofRof::RofRofException &e) {
                    res->writeStatus(e.status)->end(e.what());
                }
            })
            .get("/apps/:appId/channels/:channelName", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                try {
                    controller->fetchChannel(res, req);
                } catch (RofRof::RofRofException &e) {
                    res->writeStatus(e.status)->end(e.what());
                }
            })
            .get("/apps/:appId/channels/:channelName/users", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                try {
                    controller->fetchUsers(res, req);
                } catch (RofRof::RofRofException &e) {
                    res->writeStatus(e.status)->end(e.what());
                }
            })
            .ws<RofRof::PerUserData>("/app/:appId", {
                    /* Settings */
                    .compression = uWS::SHARED_COMPRESSOR,
                    .maxPayloadLength = 16 * 1024,
                    .idleTimeout = 35,
                    .maxBackpressure = 1 * 1024 * 1024,
                    /* Handlers */
                    .upgrade = [&](auto *res, auto *req, auto *context) {
                        websocketHandler->onUpgrade(res, req, context);
                    },
                    .open = [&](auto *ws) {
                        websocketHandler->onOpen(ws);
                    },
                    .message = [&](auto *ws, std::string_view message, uWS::OpCode opCode) {
                        websocketHandler->onMessage(ws, message, opCode);
                    },
                    .drain = [&](auto *ws) {
                        /* Check getBufferedAmount here */
                        websocketHandler->onDrain(ws);
                    },
                    .ping = [](auto *ws) {

                    },
                    .pong = [](auto *ws) {

                    },
                    .close = [&](auto *ws, int code, std::string_view message) {
                        websocketHandler->onClose(ws, code, message);
                    }
            })
            .listen(7000, [](auto *token) {
                if (token) {
                    std::cout << "Thread " << std::this_thread::get_id() << " listening on port " << 7000 << std::endl;
                } else {
                    std::cout << "Thread " << std::this_thread::get_id() << " failed to listen on port 7000"
                              << std::endl;
                }
            }).run();

}
