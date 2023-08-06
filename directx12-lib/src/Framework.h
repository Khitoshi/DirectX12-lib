#pragma once
#include <Windows.h>
#include <memory>
#include <functional>

class DX12Resources;
class ImGuiManager;

/// <summary>
/// �t���[�����[�N�N���X
/// </summary>
class Framework
{
    friend class FrameworkFactory;
private:
    Framework(const HWND& h) :
        hWnd_(h),
        dx12_resources_(),
        imgui_manager_(),
        fps_(0),
        mspf_(0)
    {}
public:
    ~Framework() {};

public:
    //���s
    int run(std::function<bool()> processMessages);
private:
    //����������
    void init();
    //�X�V����
    void update();
    //�`�揈��
    void render();
    //�f�o�b�O�`�揈��
    void debugRender();
    //�I������
    void deinit();

    //�t���[�����[�g�v�Z
    void calculateFrameStats();

private:
    HWND hWnd_;                                     //�E�B���h�E�n���h��
    std::shared_ptr<DX12Resources> dx12_resources_; //DX12���\�[�X
    std::shared_ptr<ImGuiManager> imgui_manager_;   //ImGui�}�l�[�W���[
    float fps_;                                      //FPS
    float mspf_;
};
