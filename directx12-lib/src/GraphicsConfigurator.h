#pragma once

#include <Windows.h>

struct GraphicsConfigurationData
{
    friend class GraphicsConfigurator;
private:
    float backgroundColor[4];

    UINT frameBufferCount;

    UINT windowWidth;
    UINT windowHeight;
};


class GraphicsConfigurator
{
    friend class DX12Resources;

private:
    GraphicsConfigurator() {};
    ~GraphicsConfigurator() {};

    static void init();

public://éÊìæån

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

private://ê›íËån
    void setBackgroundColor(float r, float g, float b, float a) {
        conf_data_.backgroundColor[0] = r;
        conf_data_.backgroundColor[1] = g;
        conf_data_.backgroundColor[2] = b;
        conf_data_.backgroundColor[3] = a;
    }

    void setFrameBufferCount(UINT count) {
        conf_data_.frameBufferCount = count;
    }

    void setWindowWidth(UINT width) {
        conf_data_.windowWidth = width;
    }

    void setWindowHeight(UINT height) {
        conf_data_.windowHeight = height;
    }

private:
    static GraphicsConfigurationData conf_data_;

};

