//
// Created by rocky on 11/10/20.
//

#ifndef ROFROF_CONFIGFILEREADER_H
#define ROFROF_CONFIGFILEREADER_H

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <vector>
#include <jsoncpp/json/json.h>
#include <memory>
#include "IAppManager.h"

namespace RofRof {
    template<bool SSL, bool isServer>
    struct ConfigFileReader {
        std::vector<Json::Value> apps;

    private:
        static std::string readFileContents(const std::string &path) {
            std::ifstream inFile;
            inFile.open(path);

            std::string content;

            if (inFile.is_open()) {
                while (!inFile.eof()) {
                    std::string line;
                    inFile >> line;
                    content += line;
                }
            } else {
                std::cout << "Could not open config file" << std::endl;
            }

            return content;
        }

        static Json::Value parseString(const std::string &content) {
            Json::Value config;
            JSONCPP_STRING err;

            auto message = std::string_view(content);
            Json::CharReaderBuilder builder;
            const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            if (!reader->parse(message.cbegin(), message.cend(), &config, &err)) {
                std::cout << "error" << std::endl;
                return config;
            }

            return config;
        }

        std::vector<Json::Value> mapToApps(const Json::Value &config) {
            int i = 0;
            while (config.isValidIndex(i)) {
                auto appConfig = config[i++];
                apps.push_back(appConfig);
            }

            return apps;
        }

    public:
        ConfigFileReader *read(const std::string &filePath) {
            std::string config_str = readFileContents(filePath);
            Json::Value config = parseString(config_str);
            config_str = "";
            mapToApps(config);

            return this;
        }

        ConfigFileReader *make(RofRof::IAppManager<SSL, isServer> *channelManager) {
            for (Json::Value &appConfig : apps) {
                channelManager->instantiate(appConfig);
            }

            return this;
        }
    };
}

#endif //ROFROF_CONFIGFILEREADER_H
