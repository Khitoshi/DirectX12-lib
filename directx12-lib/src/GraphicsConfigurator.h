#pragma once

#include <Windows.h>
#include <wchar.h>
#include <string>

struct GraphicsConfigurationData
{
	std::string app_name;

	//wchar_t* app_name;
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

public://éÊìæån

	//ÉVÉìÉOÉãÉgÉì
	static GraphicsConfigurator& getInstance() {
		static GraphicsConfigurator instance;
		return instance;
	}

	void init();

public:
	GraphicsConfigurationData& getConfigurationData() {
		return conf_data_;
	}


private://ê›íËån
	/*
	static void setBackgroundColor(float r, float g, float b, float a) {
		conf_data_.background_color[0] = r;
		conf_data_.background_color[1] = g;
		conf_data_.background_color[2] = b;
		conf_data_.background_color[3] = a;
	}

	static void setFrameBufferCount(UINT count) {
		conf_data_.frameBufferCount = count;
	}

	static void setWindowWidth(UINT width) {
		conf_data_.windowWidth = width;
	}

	static void setWindowHeight(UINT height) {
		conf_data_.windowHeight = height;
	}
	*/

private:
	static GraphicsConfigurationData conf_data_;

};
