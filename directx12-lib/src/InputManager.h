#pragma once
#include <Windows.h>
#include <vector>

class MouseInputListener;

class InputManager
{
private:
    InputManager() :
        mouse_input_listeners_()
    {};
    ~InputManager() {};
public:
    /// <summary>
    /// シングルトンなインスタンスを返す
    /// </summary>
    /// <returns>
    /// シングルトンなインスタンス
    /// </returns>
    static InputManager& Instance() {
        static InputManager instance;
        return instance;
    }

    // マウスのリスナーを追加する
    void addMouseInputListener(MouseInputListener* listener);
    // マウスの移動を通知する
    void OnMouseMove(int x, int y);

    // マウスの左ボタンが押されたことを通知する
    void onMouseLeftButtonDown();
    // マウスの左ボタンが離されたことを通知する
    void onMouseLeftButtonUp();
    // マウスの右ボタンが押されたことを通知する
    void onMouseRightButtonDown();
    // マウスの右ボタンが離されたことを通知する
    void onMouseRightButtonUp();

private:
    std::vector<MouseInputListener*> mouse_input_listeners_;
};
