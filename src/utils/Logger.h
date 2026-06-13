//
// Created by rocky on 11/13/20.
//

#ifndef ROFROF_LOGGER_H
#define ROFROF_LOGGER_H

#include <iostream>
#include <string>

namespace RofRof {
    struct Logger {
        static void log(const std::string &str) {
            std::cout << str << std::endl;
        }

        template<typename... Args>
        static void error(Args... args) {
            (std::cerr << ... << args) << std::endl;
        }

        template<typename... Args>
        static void debug(Args...) {
            // Debug logging is compiled out for performance.
            // To enable, replace this body with:
            //   (std::cout << ... << args) << std::endl;
        }
    };
}

#endif //ROFROF_LOGGER_H
