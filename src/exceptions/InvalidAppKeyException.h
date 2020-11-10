//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_INVALIDAPPKEYEXCEPTION_H
#define ROFROF_INVALIDAPPKEYEXCEPTION_H

#include <exception>
#include <string>
#include "RofRofException.h"

namespace RofRof {
    struct InvalidAppKeyException : public RofRof::RofRofException {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "Invalid app key provided.";
        }
    };
}

#endif //ROFROF_INVALIDAPPKEYEXCEPTION_H
