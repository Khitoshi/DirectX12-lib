#pragma once

#include <Windows.h>
#include <wchar.h>
#include <string>

struct GraphicsConfigurationData
{
	friend class GraphicsConfigurator;
private:

	std::string app_name;

	//wchar_t* app_name;
	float backgroundColor[4];

	UINT frameBufferCount;

	UINT windowWidth;
	UINT windowHeight;

	UINT positionX;
	UINT positionY;

	bool isFullScreen;
};


class GraphicsConfigurator
{
	GraphicsConfigurator() {};
	~GraphicsConfigurator() {};

public://éÊìæån
	static void init();

	static std::string getAppName() {
		return conf_data_.app_name;
	}

	static float* getBackgroundColor() {
		return conf_data_.backgroundColor;
	}

	static UINT getFrameBufferCount() {
		return conf_data_.frameBufferCount;
	}

	static UINT getWindowWidth() {
		return conf_data_.windowWidth;
	}

	static UINT getWindowHeight() {
		return conf_data_.windowHeight;
	}

	static UINT getPositionX() {
		return conf_data_.positionX;
	}

	static UINT getPositionY() {
		return conf_data_.positionY;
	}

	static bool getIsFullScreen() {
		return conf_data_.isFullScreen;
	}

private://ê›íËån
	static void setBackgroundColor(float r, float g, float b, float a) {
		conf_data_.backgroundColor[0] = r;
		conf_data_.backgroundColor[1] = g;
		conf_data_.backgroundColor[2] = b;
		conf_data_.backgroundColor[3] = a;
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

private:
	static GraphicsConfigurationData conf_data_;

};

