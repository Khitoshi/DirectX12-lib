#include "GraphicsConfigurator.h"
#include "Json.h"

GraphicsConfigurationData GraphicsConfigurator::conf_data_ = {};

void GraphicsConfigurator::init()
{
    nlohmann::json data = Json::load("./config/window.json");

    conf_data_.backgroundColor[0] = data["backgroundColor"][0];
    conf_data_.backgroundColor[1] = data["backgroundColor"][1];
    conf_data_.backgroundColor[2] = data["backgroundColor"][2];
    conf_data_.backgroundColor[3] = data["backgroundColor"][3];

    conf_data_.frameBufferCount = data["frameBufferCount"];

    conf_data_.windowWidth = data["width"];
    conf_data_.windowHeight = data["height"];
}
