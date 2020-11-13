//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_INVALIDAPPSECRETEXCEPTION_H
#define ROFROF_INVALIDAPPSECRETEXCEPTION_H

#include <exception>
#include <string>
#include "RofRofException.h"

namespace RofRof {
    struct InvalidAppSecretException : public RofRof::RofRofException {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "Invalid app secret provided.";
        }
    };
}

#endif //ROFROF_INVALIDAPPSECRETEXCEPTION_H
