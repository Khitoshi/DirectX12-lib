#include "ImGuiManager.h"
#include <stdexcept>
#include "..\OffScreenRenderTargetCacheManager.h"

/// <summary>
/// imguiの初期化
/// </summary>
/// <param name="conf">imgui生成処理の設定</param>
void ImGuiManager::init(const ImGuiManagerConf conf)
{
    //ディスクリプタヒープの作成
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

    //TODO:リファクタリング対象
    OffScreenRenderTarget::OffScreenRenderTargetConf osrtConf = {};
    osrtConf = OffScreenRenderTargetCacheManager::getInstance().getConf();
    this->offScreenRenderTarget = std::make_shared<OffScreenRenderTarget>(osrtConf);
    this->offScreenRenderTarget->init(conf.device);
}

/// <summary>
/// Frame開始処理
/// </summary>
void ImGuiManager::beginFrame(RenderContext* rc, ID3D12Device* device)
{
    //オフスクリーンレンダーターゲットで書き込みできる状態にする
    auto renderTarget = offScreenRenderTarget->getRTVHeap();
    auto resource = offScreenRenderTarget->getResource();
    auto depthStencil = OffScreenRenderTargetCacheManager::getInstance().getDepthStencilViewHandle();
    auto viewport = OffScreenRenderTargetCacheManager::getInstance().getViewport();

    //ビューポートとシザリング矩形の設定
    rc->transitionOffScreenRenderTargetBegin(resource);
    rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depthStencil, resource);

    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

/// <summary>
/// Frame終了処理
/// </summary>
void ImGuiManager::endFrame()
{
    ImGui::Render();
}

/// <summary>
/// imguiの描画
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
void ImGuiManager::render(RenderContext* rc, ID3D12Device* device)
{
    ImGui::Render();
    rc->setDescriptorHeap(this->descriptorHeap.Get());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), rc->getCommandList());

    //TODO:リファクタリング対象
    //オフスクリーンレンダーターゲットの書き込みを終了する。
    offScreenRenderTarget->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(offScreenRenderTarget.get());
}

/// <summary>
/// 解放処理
/// </summary>
void ImGuiManager::deinit()
{
    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

/// <summary>
/// ディスクリプタヒープの作成
/// </summary>
/// <param name="conf">imgui生成処理の設定</param>
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