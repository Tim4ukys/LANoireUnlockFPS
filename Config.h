#pragma once

#include "json.hpp"

class Config {
	Config() = delete;
    Config(Config&) = delete;
    Config(Config&&) = delete;
public:
    using json = nlohmann::json;

    Config(const char* fileName);

    inline json& operator[](const char* key) {
        return m_JSON[key]; 
    }

private:

    mutable json m_JSON;
};
