#include "GraphicsConfigurator.h"
#include "Json.h"

#include <Windows.h>
GraphicsConfigurationData GraphicsConfigurator::conf_data_ = {};

void GraphicsConfigurator::init()
{
	nlohmann::json data = Json::load("./config/window.json");

	conf_data_.app_name = data["title"];

	conf_data_.background_color[0] = data["backgroundColor"][0];
	conf_data_.background_color[1] = data["backgroundColor"][1];
	conf_data_.background_color[2] = data["backgroundColor"][2];
	conf_data_.background_color[3] = data["backgroundColor"][3];

	conf_data_.frame_buffer_count = data["frameBufferCount"];

	conf_data_.window_width = data["width"];
	conf_data_.window_height = data["height"];

	conf_data_.position_x = data["positionX"];
	conf_data_.position_y = data["positionY"];

	conf_data_.is_fullscreen = data["isFullScreen"];

}
