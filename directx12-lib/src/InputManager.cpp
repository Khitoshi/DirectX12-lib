#include "InputManager.h"
#include "MouseInputListener.h"


/// <summary>
/// マウスのリスナーを追加する
/// </summary>
/// <param name="listener"></param>
void InputManager::addMouseInputListener(MouseInputListener* listener)
{
    //すでに登録されている場合は何もしない
    auto itr = std::find(mouse_input_listeners_.begin(), mouse_input_listeners_.end(), listener);
    if (itr != mouse_input_listeners_.end()) {
        return;
    }

    this->mouse_input_listeners_.push_back(listener);
}

/// <summary>
/// マウスの移動を通知する
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void InputManager::OnMouseMove(int x, int y)
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseMove(x, y);
    }
}

/// <summary>
/// マウスの左ボタンが押されたことを通知する
/// </summary>
void InputManager::onMouseLeftButtonDown()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseLeftButtonDown();
    }
}

/// <summary>
/// マウスの左ボタンが離されたことを通知する
/// </summary>
void InputManager::onMouseLeftButtonUp()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseLeftButtonUp();
    }
}

/// <summary>
/// マウスの右ボタンが押されたことを通知する
/// </summary>
void InputManager::onMouseRightButtonDown()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseRightButtonDown();
    }
}

/// <summary>
/// マウスの右ボタンが離されたことを通知する
/// </summary>
void InputManager::onMouseRightButtonUp()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseRightButtonUp();
    }
}
