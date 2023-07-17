#pragma once

#include "../d3dx12.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "../RenderContext.h"
using namespace Microsoft::WRL;

/// <summary>
/// imgui生成処理の設定
/// </summary>
struct ImGuiManagerConf {
    HWND hWnd;
    ID3D12Device* device;
    UINT frameBufferCount;
};

/// <summary>
/// imguiの管理クラス
/// </summary>
class ImGuiManager
{
public:
    ImGuiManager() :
        descriptorHeap()
    {};
    ~ImGuiManager() { deinit(); };

    //初期化処理
    void init(const ImGuiManagerConf conf);
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
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const ImGuiManagerConf conf);
private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //ディスクリプタヒープ
};
