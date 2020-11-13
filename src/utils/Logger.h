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
            (std::cout << ... << args) << std::endl;
        }

        template<typename... Args>
        static void debug(Args... args) {
            return;
            (std::cout << ... << args) << std::endl;
        }
    };
}

#endif //ROFROF_LOGGER_H
