#include <thread>
#include <iostream>
#include "App.h"
#include "PerUserData.h"
#include "WebSocketHandler.h"

int main() {
    auto *websocketHandler = new RofRof::WebSocketHandler<false, true>();

    uWS::App()
            .get("/*", [](auto *res, auto *req) {
                res->end("Hello world!");
            })
            .ws<RofRof::PerUserData>("/*", {
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
