#pragma once

#include <Windows.h>
#include <memory>
#include "MouseInputListener.h"
#include "Camera.h"



class CameraController :public MouseInputListener
{
public:
    CameraController(Camera* camera) :
        camera_(camera),
        is_left_button_down_(false),
        is_right_button_down_(false)
    {}
    ~CameraController() {}

    void onMouseMove(int x, int y) override;

    //void onKeyDown(LPARAM lParam) override;

    void onMouseLeftButtonDown() override;
    void onMouseLeftButtonUp() override;
    void onMouseRightButtonDown() override;
    void onMouseRightButtonUp() override;

private:
    Camera* camera_;
    bool is_left_button_down_;
    bool is_right_button_down_;
};
