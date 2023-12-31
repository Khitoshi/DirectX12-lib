#pragma once

#include "d3dx12.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

class RenderContext;
class DepthStencil;
class OffScreenRenderTarget;
class DescriptorHeap;

using namespace Microsoft::WRL;

/// <summary>
/// imguiの管理クラス
/// </summary>
class ImGuiManager
{
public:
    ImGuiManager() :
        descriptor_heap_(nullptr),
        off_screen_render_target_(nullptr),
        depth_stencil_(nullptr)
    {};
    ~ImGuiManager() {};

    //初期化処理
    void init(ID3D12Device* device, const HWND& hWnd);
    //フレーム開始処理
    void beginFrame(RenderContext* rc, ID3D12Device* device);
    //フレーム終了処理
    void endFrame();
    //描画処理
    void render(RenderContext* rc, ID3D12Device* device);
    //解放処理
    void deinit();
private:
    //ディスクリプタヒープ生成
    void createDescriptorHeap(ID3D12Device* device);
    //オフスクリーンレンダーターゲット生成
    void createOffScreenRenderTarget(ID3D12Device* device);
    //深度ステンシル生成
    void createDepthStencil(ID3D12Device* device);
private:
    std::shared_ptr<DescriptorHeap> descriptor_heap_;                      //ディスクリプタヒープ
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;    //オフスクリーンレンダーターゲット
    std::shared_ptr<DepthStencil> depth_stencil_;                       //深度ステンシル
};
