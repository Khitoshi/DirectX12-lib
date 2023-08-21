#include "ImGuiManager.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "CommonGraphicsConfig.h"
#include "OffScreenRenderTargetFactory.h"
#include "DepthStencilCacheManager.h"
#include "RenderContext.h"
#include "DescriptorHeapFactory.h"
#include <stdexcept>

/// <summary>
/// imgui�̏�����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
/// <param name="hWnd">�E�B���h�E�n���h���̃C���^�[�t�F�[�X</param>
void ImGuiManager::init(ID3D12Device* device, const HWND& hWnd)
{
    createDescriptorHeap(device);
    createOffScreenRenderTarget(device);
    createDepthStencil(device);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX12_Init(
        device,
        frameBufferCount,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        this->descriptor_heap_->getDescriptorHeap(),
        this->descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
        this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
}

/// <summary>
/// Frame�J�n����
/// </summary>
void ImGuiManager::beginFrame(RenderContext* rc, ID3D12Device* device)
{
    //�I�t�X�N���[�������_�[�^�[�Q�b�g�ŏ������݂ł����Ԃɂ���
    auto render_target = this->off_screen_render_target_->getRTVHeap();
    auto resource = off_screen_render_target_->getResource();
    auto depth_stencil = depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

    //�r���[�|�[�g�ƃV�U�����O��`�̐ݒ�
    rc->transitionOffScreenRenderTargetBegin(resource);
    rc->simpleStart(render_target->GetCPUDescriptorHandleForHeapStart(), depth_stencil);

    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

/// <summary>
/// Frame�I������
/// </summary>
void ImGuiManager::endFrame()
{
    ImGui::Render();
}

/// <summary>
/// imgui�̕`��
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
/// <param name="device">GPU�f�o�C�X</param>
void ImGuiManager::render(RenderContext* rc, ID3D12Device* device)
{
    ImGui::Render();
    rc->setDescriptorHeap(this->descriptor_heap_.get());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), rc->getCommandList());

    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̏������݂��I������B
    this->off_screen_render_target_->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(this->off_screen_render_target_.get());
}

/// <summary>
/// �������
/// </summary>
void ImGuiManager::deinit()
{
    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̍쐬
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void ImGuiManager::createDescriptorHeap(ID3D12Device* device)
{
    this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

void ImGuiManager::createOffScreenRenderTarget(ID3D12Device* device)
{
    //TODO:���t�@�N�^�����O�Ώ�
    OffScreenRenderTarget::OffScreenRenderTargetConf osrt_conf = {};
    {//�f�B�X�N���v�^�q�[�v�̐ݒ�
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = 1;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0;

        osrt_conf.descriptor_heap_desc = desc;
    }
    {//���\�[�X�f�X�N�̐ݒ�
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = windowWidth;
        desc.Height = windowHeight;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        osrt_conf.resource_desc = desc;
    }

    this->off_screen_render_target_ = OffScreenRenderTargetFactory::create(osrt_conf, device);
}

void ImGuiManager::createDepthStencil(ID3D12Device* device)
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.frame_buffer_count = frameBufferCount;
    ds_conf.width = windowWidth;
    ds_conf.height = windowHeight;
    this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}