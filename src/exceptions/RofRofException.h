//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_ROFROFEXCEPTION_H
#define ROFROF_ROFROFEXCEPTION_H

#include <exception>
#include <string>

namespace RofRof {
    struct RofRofException : public std::exception {
    public:
        std::string status = "401 Unauthorized";
    };
}

#endif //ROFROF_ROFROFEXCEPTION_H
