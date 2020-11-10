//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_INVALIDAPPIDEXCEPTION_H
#define ROFROF_INVALIDAPPIDEXCEPTION_H

#include <exception>
#include <string>
#include "RofRofException.h"

namespace RofRof {
    struct InvalidAppIdException : public RofRof::RofRofException {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "Invalid app id provided.";
        }
    };
}

#endif //ROFROF_INVALIDAPPIDEXCEPTION_H
