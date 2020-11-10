//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_SIGNATUREMISMATCHEXCEPTION_H
#define ROFROF_SIGNATUREMISMATCHEXCEPTION_H

#include <exception>
#include <string>
#include "RofRofException.h"

namespace RofRof {
    struct SignatureMismatchException : public RofRof::RofRofException {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "Invalid auth signature provided.";
        }
    };
}

#endif //ROFROF_SIGNATUREMISMATCHEXCEPTION_H
