//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_UTILS_H
#define ROFROF_UTILS_H

#include <string>
#include <vector>
#include <iomanip>
#include <openssl/hmac.h>
#include <openssl/md5.h>


namespace RofRof {
    struct Strings {
        static std::vector<std::string> explode(const std::string &str, const char &ch) {
            std::string next;
            std::vector<std::string> result;

            // For each character in the string
            for (char it : str) {
                // If we've hit the terminal character
                if (it == ch) {
                    // If we have some characters accumulated
                    if (!next.empty()) {
                        // Add them to the result vector
                        result.push_back(next);
                        next.clear();
                    }
                } else {
                    // Accumulate the next character into the sequence
                    next += it;
                }
            }
            if (!next.empty())
                result.push_back(next);
            return result;
        }

        static std::string implode_map(const char &glue, const char &separator, const std::map<std::string, std::string> &params) {
            std::string str;

            for (auto &param : params) {
                str += param.first;
                str += glue;
                str += param.second;
                str += separator;
            }

            str.pop_back();

            return str;
        }

        static std::string hmac_sha256(const std::string &key, const std::string &msg) {
            unsigned char hash[32];

            HMAC_CTX *ctx = HMAC_CTX_new();
            HMAC_Init_ex(ctx, &key[0], (int) key.length(), EVP_sha256(), nullptr);
            HMAC_Update(ctx, (unsigned char *) &msg[0], msg.length());
            unsigned int len = 32;
            HMAC_Final(ctx, hash, &len);
            HMAC_CTX_free(ctx);

            return char_array_to_string(hash, len);
        }

        static std::string md5(const std::string &content) {
            unsigned char body_md5[MD5_DIGEST_LENGTH];
            MD5((unsigned char *) content.c_str(), content.size(), body_md5);
            return char_array_to_string(body_md5, MD5_DIGEST_LENGTH);
        }

        static std::string char_array_to_string(unsigned char *array, unsigned int len) {
            std::string str;
            for (int i = 0; i < len; i++) {
                char c[3];
                std::sprintf(c, "%02x", array[i]);
                str += c;
            }

            return str;
        }
    };
}

#endif //ROFROF_UTILS_H
