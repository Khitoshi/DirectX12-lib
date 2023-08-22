#pragma once

class MouseInputListener
{
public:
    MouseInputListener() {};
    ~MouseInputListener() {};

    virtual void onMouseMove(int x, int y) = 0;
    virtual void onMouseLeftButtonDown() = 0;
    virtual void onMouseLeftButtonUp() = 0;
    virtual void onMouseRightButtonDown() = 0;
    virtual void onMouseRightButtonUp() = 0;


    //virtual void onKeyDown(LPARAM lParam) = 0;

    //virtual void onLeftButtonDown() = 0;
    //virtual void onLeftButtonUp() = 0;
    //virtual void onRightButtonDown() = 0;
    //virtual void onRightButtonUp() = 0;
    //virtual void onMiddleButtonDown() = 0;
    //virtual void onMiddleButtonUp() = 0;
    //virtual void onWheelScroll(int delta) = 0;
    //virtual void onMouseMove(int x, int y) = 0;
};
