#include "App.h"
#include "PerUserData.h"
#include <thread>
#include <algorithm>
#include "Channel.h"
#include "WebSocketHandler.h"
#include "ClientMessage.h"

int main() {
    std::vector<std::thread *> threads(std::thread::hardware_concurrency());

    std::transform(threads.begin(), threads.end(), threads.begin(), [](std::thread *t) {
        return new std::thread([]() {

            uWS::App()
                    .get("/*", [](auto *res, auto *req) {
                        res->end("Hello world!");
                    })
                    .ws<RofRof::PerUserData>("/*", {
                            /* Settings */
                            .compression = uWS::SHARED_COMPRESSOR,
                            .maxPayloadLength = 16 * 1024,
                            .idleTimeout = 10,
                            .maxBackpressure = 1 * 1024 * 1024,
                            /* Handlers */
                            .open = [](auto *ws) {

                            },
                            .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                                ws->send(message, opCode);
                            },
                            .drain = [](auto *ws) {
                                /* Check getBufferedAmount here */
                            },
                            .ping = [](auto *ws) {

                            },
                            .pong = [](auto *ws) {

                            },
                            .close = [](auto *ws, int code, std::string_view message) {

                            }
                    })
                    .listen(3000, [](auto *token) {
                        if (token) {
                            std::cout << "Thread " << std::this_thread::get_id() << " listening on port " << 3000
                                      << std::endl;
                        } else {
                            std::cout << "Thread " << std::this_thread::get_id() << " failed to listen on port 3000"
                                      << std::endl;
                        }
                    }).run();

        });
    });

    std::for_each(threads.begin(), threads.end(), [](std::thread *t) {
        t->join();
    });
}
