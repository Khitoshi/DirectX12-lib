#pragma once

#include "../d3dx12.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

class RenderContext;
class DepthStencil;
class OffScreenRenderTarget;

using namespace Microsoft::WRL;

/// <summary>
/// imgui�̊Ǘ��N���X
/// </summary>
class ImGuiManager
{
public:
    /// <summary>
    /// imgui���������̐ݒ�
    /// </summary>
    struct ImGuiManagerConf {
        HWND hWnd;
    };

public:
    ImGuiManager(ImGuiManagerConf c) :
        conf_(c),
        descriptor_heap_(nullptr),
        off_screen_render_target_(nullptr),
        depth_stencil_(nullptr)
    {};
    ~ImGuiManager() { deinit(); };

    //����������
    void init(ID3D12Device* device);
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
    void createDescriptorHeap(ID3D12Device* device);
    //�I�t�X�N���[�������_�[�^�[�Q�b�g����
    void createOffScreenRenderTarget(ID3D12Device* device);
    //�[�x�X�e���V������
    void createDepthStencil(ID3D12Device* device);
private:
    ImGuiManagerConf conf_;                                             //�ݒ�
    ComPtr<ID3D12DescriptorHeap> descriptor_heap_;                      //�f�B�X�N���v�^�q�[�v
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;    //�I�t�X�N���[�������_�[�^�[�Q�b�g
    std::shared_ptr<DepthStencil> depth_stencil_;                       //�[�x�X�e���V��
};
