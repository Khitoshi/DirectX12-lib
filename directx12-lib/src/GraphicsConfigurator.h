#pragma once

#include <Windows.h>
#include <wchar.h>
#include <string>

struct GraphicsConfigurationData
{
	std::string app_name;

	float background_color[4];

	UINT frame_buffer_count;

	UINT window_width;
	UINT window_height;

	UINT position_x;
	UINT position_y;

	bool is_fullscreen;
};


class GraphicsConfigurator
{
	GraphicsConfigurator() {};
	~GraphicsConfigurator() {};

public://Žæ“¾Œn
	static GraphicsConfigurator& getInstance() {
		static GraphicsConfigurator instance;
		return instance;
	}

	void init();

public:
	GraphicsConfigurationData& getConfigurationData() {
		return conf_data_;
	}

private:
	static GraphicsConfigurationData conf_data_;

};
