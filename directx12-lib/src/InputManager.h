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
    /// �V���O���g���ȃC���X�^���X��Ԃ�
    /// </summary>
    /// <returns>
    /// �V���O���g���ȃC���X�^���X
    /// </returns>
    static InputManager& Instance() {
        static InputManager instance;
        return instance;
    }

    // �}�E�X�̃��X�i�[��ǉ�����
    void addMouseInputListener(MouseInputListener* listener);
    // �}�E�X�̈ړ���ʒm����
    void OnMouseMove(int x, int y);

    // �}�E�X�̍��{�^���������ꂽ���Ƃ�ʒm����
    void onMouseLeftButtonDown();
    // �}�E�X�̍��{�^���������ꂽ���Ƃ�ʒm����
    void onMouseLeftButtonUp();
    // �}�E�X�̉E�{�^���������ꂽ���Ƃ�ʒm����
    void onMouseRightButtonDown();
    // �}�E�X�̉E�{�^���������ꂽ���Ƃ�ʒm����
    void onMouseRightButtonUp();

private:
    std::vector<MouseInputListener*> mouse_input_listeners_;
};
