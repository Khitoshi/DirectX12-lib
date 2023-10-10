#pragma once

#include <nlohmann/json.hpp>

class Json
{
private:
    Json() {};
    ~Json() {};
public:
    static nlohmann::json load(const char* file_path);
};
