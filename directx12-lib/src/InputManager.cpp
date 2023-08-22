#include "InputManager.h"
#include "MouseInputListener.h"


/// <summary>
/// �}�E�X�̃��X�i�[��ǉ�����
/// </summary>
/// <param name="listener"></param>
void InputManager::addMouseInputListener(MouseInputListener* listener)
{
    //���łɓo�^����Ă���ꍇ�͉������Ȃ�
    auto itr = std::find(mouse_input_listeners_.begin(), mouse_input_listeners_.end(), listener);
    if (itr != mouse_input_listeners_.end()) {
        return;
    }

    this->mouse_input_listeners_.push_back(listener);
}

/// <summary>
/// �}�E�X�̈ړ���ʒm����
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
/// �}�E�X�̍��{�^���������ꂽ���Ƃ�ʒm����
/// </summary>
void InputManager::onMouseLeftButtonDown()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseLeftButtonDown();
    }
}

/// <summary>
/// �}�E�X�̍��{�^���������ꂽ���Ƃ�ʒm����
/// </summary>
void InputManager::onMouseLeftButtonUp()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseLeftButtonUp();
    }
}

/// <summary>
/// �}�E�X�̉E�{�^���������ꂽ���Ƃ�ʒm����
/// </summary>
void InputManager::onMouseRightButtonDown()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseRightButtonDown();
    }
}

/// <summary>
/// �}�E�X�̉E�{�^���������ꂽ���Ƃ�ʒm����
/// </summary>
void InputManager::onMouseRightButtonUp()
{
    for (auto& mouse_listener : this->mouse_input_listeners_) {
        mouse_listener->onMouseRightButtonUp();
    }
}
