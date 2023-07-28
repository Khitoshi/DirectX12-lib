#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include "d3dx12.h"
#include <dxgi1_4.h>

class DeviceContext;
class CommandQueue;
class SwapChain;
class CommandAllocator;
class GraphicsCommandList;
class RenderTarget;

class CompositeRenderTarget;
class DepthStencil;
class RenderContext;
class Fence;
class FullScreenQuad;

using namespace Microsoft::WRL;

/// <summary>
/// DirectX12のリソースを纏めて処理するクラス
/// </summary>
class DX12Resources
{
public:
    DX12Resources() :
        device_context_(nullptr),
        command_queue_(),
        swap_chain_(),
        command_allocator_(),
        command_list_(),
        render_target_(),
        composite_render_target_(),
        depth_stencil_(),
        fence_(),
        viewport_(),
        scissor_rect_(),
        render_context_(),
        current_frame_buffer_rtv_handle_(),
        current_frame_buffer_dsv_handle_(),
        frame_index_(0),
        full_screen_quad_()
    {}
    ~DX12Resources() { waitEndOfDrawing(); }

    //初期化処理
    void init(const HWND hWnd, const int width, const int height);

    //レンダリング開始処理
    void beginRender();

    //レンダリング終了処理
    void endRender();

    //描画の終了を待機する
    void waitEndOfDrawing();

private://生成系
    //IDXGIファクトリ生成
    ComPtr<IDXGIFactory4> createFactory();
    void initCommandQueue();
    //スワップチェイン生成
    void initSwapChain(const HWND hWnd, const int width, const int height, const int frameBufferCount, IDXGIFactory4* factory);
    //コマンドアロケータ生成
    void initCommandAllocator();
    //コマンドリスト生成
    void initCommandList();
    //Mainレンダーターゲット生成
    void initRenderTarget();
    //オフスクリーンレンダーターゲット生成
    void initCompositeRenderTarget();
    //深度ステンシル生成
    void initDepthStencil(const int width, const int height);
    //フェンス生成
    void initFence();
    //ビューポート設定
    void initViewport(const int width, const int height);
    //シザリング矩形設定
    void initScissorRect(const int width, const int height);
    //レンダーコンテキスト生成
    void initRenderContext();
    //フルスクリーン四角形生成
    void initFullScreenQuad();

    //オフスクリーンレンダーターゲットの設定生成
    void createOffScreenRenderTargetConf();

    //レンダーターゲットビューのハンドルを設定
    void setMainRTVHandle();
    //オフスクリーンレンダーターゲットビューのハンドルを設定
    void setOffScreenRTVHandle();
    //深度ステンシルビューのハンドルを設定
    void updateDSVHandle();
    //オフスクリーンレンダーターゲットの設定を更新
    void updateOffScreenRenderTargetConf();

public://取得系
    //デバイス取得
    DeviceContext* getDeviceContext() const { return device_context_.get(); }
    //レンダーコンテキスト取得
    RenderContext* getRenderContext() const { return render_context_.get(); }
    //ビューポート取得
    D3D12_VIEWPORT getViewport() const { return viewport_; }
    //現在書き込み中のフレームバッファの深度ステンシルビューのハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE getCurrentFrameBufferDSVHandle() const { return current_frame_buffer_dsv_handle_; }

private:
    std::shared_ptr<DeviceContext>device_context_;                                   //デバイス
    std::shared_ptr<CommandQueue>command_queue_;                       //コマンドキュー
    std::shared_ptr<SwapChain>swap_chain_;                          //スワップチェイン
    std::shared_ptr<CommandAllocator>command_allocator_;               //コマンドアロケータ
    std::shared_ptr<GraphicsCommandList> command_list_;
    std::shared_ptr<RenderTarget>render_target_;                    //レンダーターゲット
    std::shared_ptr<CompositeRenderTarget>composite_render_target_; //オフスクリーンレンダーターゲット
    std::shared_ptr<DepthStencil>depth_stencil_;                    //深度ステンシル
    std::shared_ptr<Fence> fence_;                                  //フェンス
    D3D12_VIEWPORT viewport_;                                       //ビューポート
    D3D12_RECT scissor_rect_;                                       //シザリング矩形
    std::shared_ptr<RenderContext> render_context_;                 //レンダーコンテキスト




    D3D12_CPU_DESCRIPTOR_HANDLE current_frame_buffer_rtv_handle_;	//現在書き込み中のフレームバッファのレンダーターゲットビューのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE current_frame_buffer_dsv_handle_;	//現在書き込み中のフレームバッファの深度ステンシルビューのハンドル

    int frame_index_;                                               //フレームの番号
    std::shared_ptr<FullScreenQuad> full_screen_quad_;              //フルスクリーン四角形
};
