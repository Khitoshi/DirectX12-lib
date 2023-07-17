#pragma once

#include "../d3dx12.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "../RenderContext.h"
using namespace Microsoft::WRL;

/// <summary>
/// imgui���������̐ݒ�
/// </summary>
struct ImGuiManagerConf {
    HWND hWnd;
    ID3D12Device* device;
    UINT frameBufferCount;
};

/// <summary>
/// imgui�̊Ǘ��N���X
/// </summary>
class ImGuiManager
{
public:
    ImGuiManager() :
        descriptorHeap()
    {};
    ~ImGuiManager() { deinit(); };

    //����������
    void init(const ImGuiManagerConf conf);
    //�t���[���J�n����
    void beginFrame(RenderContext* rc, ID3D12Device* device);
    //�t���[���I������
    void endFrame();
    //�`�揈��
    void render(RenderContext* rc, ID3D12Device* device);
    //�������
    void deinit();
private:
    //�f�B�X�N���v�^�q�[�v����
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const ImGuiManagerConf conf);
private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //�f�B�X�N���v�^�q�[�v
};
