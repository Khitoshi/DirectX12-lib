#include "ImGuiManager.h"
#include <stdexcept>
#include "..\OffScreenRenderTargetCacheManager.h"

/// <summary>
/// imgui�̏�����
/// </summary>
/// <param name="conf">imgui���������̐ݒ�</param>
void ImGuiManager::init(const ImGuiManagerConf conf)
{
    //�f�B�X�N���v�^�q�[�v�̍쐬
    this->descriptorHeap = createDescriptorHeap(conf);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(conf.hWnd);
    ImGui_ImplDX12_Init(
        conf.device,
        conf.frameBufferCount,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        this->descriptorHeap.Get(),
        this->descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        this->descriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

/// <summary>
/// Frame�J�n����
/// </summary>
void ImGuiManager::beginFrame(RenderContext* rc, ID3D12Device* device)
{
    auto renderTarget = OffScreenRenderTargetCacheManager::getInstance().getOrCreate(device);
    auto depthStencil = OffScreenRenderTargetCacheManager::getInstance().getDepthStencilViewHandle();
    auto viewport = OffScreenRenderTargetCacheManager::getInstance().getViewport();
    //�r���[�|�[�g�ƃV�U�����O��`�̐ݒ�
    rc->simpleStart(renderTarget.get()->getRTVHeap()->GetCPUDescriptorHandleForHeapStart(), depthStencil, renderTarget->getResource());

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
void ImGuiManager::render(RenderContext* rc, ID3D12Device* device)
{
    ImGui::Render();
    rc->setDescriptorHeap(this->descriptorHeap.Get());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), rc->getCommandList());

    auto renderTarget = OffScreenRenderTargetCacheManager::getInstance().getOrCreate(device);
    rc->TransitionTemporaryRenderTargetAwait(renderTarget->getResource());
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(renderTarget.get());
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
/// <param name="conf">imgui���������̐ݒ�</param>
/// <returns></returns>
ComPtr<ID3D12DescriptorHeap> ImGuiManager::createDescriptorHeap(const ImGuiManagerConf conf)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = conf.frameBufferCount;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ComPtr<ID3D12DescriptorHeap> heap;
    if (FAILED(conf.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)))) {
        throw std::runtime_error("Failed to create descriptor heap");
    }

    return heap;
}