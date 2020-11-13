#include <thread>
#include <csignal>
#include <iostream>
#include "App.h"
#include "HttpParser.h"
#include "HttpResponse.h"
#include "PerUserData.h"
#include "WebSocketHandler.h"
#include "RofRofController.h"
#include "apps/ConfigFileReader.h"

std::vector<us_listen_socket_t *> sockets;

void cleanup(int s) {
    std::cout << "Shutting down..." << std::endl;
    for (auto &socket:sockets) {
        us_listen_socket_close(0, socket);
    }
}

int main() {
    const bool SSL = false;
    const bool isServer = true;

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = cleanup;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);

    auto *websocketHandler = new RofRof::WebSocketHandler<SSL, isServer>();
    auto *controller = new RofRof::RofRofController<SSL, isServer>(websocketHandler);

    auto *configReader = new RofRof::ConfigFileReader<SSL, isServer>();
    configReader->read("apps.json")->make(websocketHandler->appManager);

    std::vector<std::thread *> threads(std::thread::hardware_concurrency());

    std::transform(threads.begin(), threads.end(), threads.begin(), [&](std::thread *t) {
        return new std::thread([&]() {
            uWS::App()
                    .get("/*", [](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                        res->end("The fastest WebSocket server in the world!!!");
                    })
                    .post("/apps/:appId/events", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                        controller->triggerEvent(res, req);
                    })
                    .get("/apps/:appId/channels", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                        controller->fetchChannels(res, req);
                    })
                    .get("/apps/:appId/channels/:channelName", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                        controller->fetchChannel(res, req);
                    })
                    .get("/apps/:appId/channels/:channelName/users", [&](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) {
                        controller->fetchUsers(res, req);
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
                                std::cout << "Thread: " << std::this_thread::get_id() << std::endl;
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
                            sockets.push_back(token);
                            std::cout << "Thread " << std::this_thread::get_id() << " listening on port " << 7000 << std::endl;
                        } else {
                            std::cout << "Thread " << std::this_thread::get_id() << " failed to listen on port 7000" << std::endl;
                        }
                    }).run();
        });
    });

    std::for_each(threads.begin(), threads.end(), [](std::thread *t) {
        t->join();
    });

    delete websocketHandler;
    delete controller;
    delete configReader;

    return 0;
}
