#include "DX12Resources.h"

#include "DeviceContextFactory.h"
#include "CommandQueueFactory.h"
#include "SwapChainFactory.h"
#include "CommandAllocatorFactory.h"
#include "GraphicsCommandListFactory.h"
#include "RenderTargetFactory.h"
#include "CompositeRenderTargetFactory.h"
#include "DepthStencilCacheManager.h"
#include "FenceFactory.h"
#include "RenderContextFactory.h"
#include "FullScreenQuadFactory.h"
#include "CommonGraphicsConfig.h"
#include <stdexcept>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="hWnd">windowハンドル</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
void DX12Resources::init(const HWND hWnd)
{
    //所有権が変わるので解放は合計で1回になる
    ComPtr<IDXGIFactory4> factory = createFactory();
    this->device_context_ = DeviceContextFactory::create(factory.Get());
    initCommandQueue();
    initSwapChain(hWnd, factory.Get());
    initCommandAllocator();
    initCommandList();
    initRenderTarget();
    initCompositeRenderTarget();
    initDepthStencil();
    initFence();
    initViewport();
    initScissorRect();
    initRenderContext();
    initFullScreenQuad();
}

/// <summary>
/// レンダリング開始処理
/// </summary>
/// <param name="color">clear color</param>
void DX12Resources::beginRender()
{
    //バックバッファインデックスの取得
    this->frame_index_ = this->swap_chain_->getCurrentBackBufferIndex();

    //コマンドアロケータのリセット
    this->command_allocator_->reset();

    //コマンドリストのリセット
    this->render_context_->reset(this->command_allocator_->GetAllocator(), nullptr);

    //深度ステンシルビューのハンドルを設定
    this->updateDSVHandle();
}

/// <summary>
/// レンダリング終了処理
/// </summary>
void DX12Resources::endRender()
{
    //offscreenを1枚のテクスチャにまとめる描画開始処理
    this->composite_render_target_->beginRender(this->render_context_.get(), this->current_frame_buffer_dsv_handle_);
    //offscreenを1枚のテクスチャにまとめる描画処理
    this->composite_render_target_->render(this->render_context_.get(), this->device_context_->getDevice());
    //offscreenを1枚のテクスチャにまとめる描画終了処理
    this->composite_render_target_->endRender(this->render_context_.get());

    //Mainレンダーターゲットの描画開始

    //レンダーターゲットの設定
    this->setMainRTVHandle();

    //Fullスクリーン四角形の描画開始処理
    this->render_context_->transitionMainRenderTargetBegin(this->render_target_->getResource(this->frame_index_));
    this->render_context_->simpleStart(this->current_frame_buffer_rtv_handle_, this->current_frame_buffer_dsv_handle_);

    //offscreenをテクスチャとしたフルスクリーン四角形を描画
    full_screen_quad_->draw(this->render_context_.get(), this->device_context_->getDevice(), composite_render_target_.get());

    //Mainレンダーターゲットの描画完了を待つ
    this->render_context_->transitionMainRenderTargetEnd(this->render_target_->getResource(this->frame_index_));

    //リソースバリアの実行
    this->render_context_->executeResourceBarriers();

    //コマンドのクローズ
    this->render_context_->close();

    //コマンドの実行
    ID3D12CommandList* cmd_list[] = { this->command_list_->GetCommandList() };
    this->command_queue_->getCommandQueue()->ExecuteCommandLists(_countof(cmd_list), cmd_list);

    //スワップチェインの更新
    this->swap_chain_->present();

    //描画完了を待つ
    waitEndOfDrawing();
}

/// <summary>
/// 描画の終了を待つ
/// </summary>
void DX12Resources::waitEndOfDrawing()
{
    //フェンスのシグナル
    this->command_queue_->getCommandQueue()->Signal(this->fence_->getFence(), this->fence_->getValue());

    //フェンスの待機
    if (this->fence_->getFence()->GetCompletedValue() < this->fence_->getValue()) {
        this->fence_->getFence()->SetEventOnCompletion(this->fence_->getValue(), this->fence_->getEvent());
        WaitForSingleObject(this->fence_->getEvent(), INFINITE);
    }

    //フェンスの値を更新
    this->fence_->incrementValue();
}

/// <summary>
/// ファクトリ　生成
/// </summary>
/// <returns>
/// 生成したファクトリのポイント
/// </returns>
ComPtr<IDXGIFactory4> DX12Resources::createFactory() {
    UINT dxgi_factory_flags = 0;
#ifdef _DEBUG
    //デバッグコントローラーがあれば、デバッグレイヤーがあるDXGIを作成する。
    ComPtr<ID3D12Debug> debug_controller;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
        debug_controller->EnableDebugLayer();

        //デバッグの場合デバッグフラグを立てる
        dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;

#if 0 // GBV を有効化する場合.
        ComPtr<ID3D12Debug3> debug3;
        debug.As(&debug3);
        if (debug3) {
            debug3->SetEnableGPUBasedValidation(true);
        }

#endif
    }
    else {
        throw std::runtime_error("failed to create debug Controller");
    }

#endif
    //factory生成
    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("failed to create dxgi factory");
    }

    return factory;
}

/// <summary>
/// コマンドキューの生成
/// </summary>
void DX12Resources::initCommandQueue()
{
    //コマンドキューの設定
    D3D12_COMMAND_QUEUE_DESC cq_desc = {};
    cq_desc.NodeMask = 0;
    cq_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cq_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    this->command_queue_ = CommandQueueFactory::create(this->device_context_->getDevice(), cq_desc);
}

/// <summary>
/// スワップチェイン生成
/// </summary>
/// <param name="hWnd">ウィンドウハンドル</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="factory">dxgiファクトリ</param>
void DX12Resources::initSwapChain(const HWND hWnd, IDXGIFactory4* factory) {
    SwapChain::SwapChainConf sc_conf = {};
    sc_conf.hWnd = hWnd;
    sc_conf.width = windowWidth;
    sc_conf.height = windowHeight;
    sc_conf.frame_buffer_count = frameBufferCount;
    sc_conf.factory = factory;
    sc_conf.command_queue = this->command_queue_->getCommandQueue();
    this->swap_chain_ = SwapChainFactory::create(sc_conf);
}

/// <summary>
/// コマンドアロケータ生成
/// </summary>
void DX12Resources::initCommandAllocator()
{
    this->command_allocator_ = CommandAllocatorFactory::create(this->device_context_->getDevice());
}

/// <summary>
/// コマンドリスト生成
/// </summary>
void DX12Resources::initCommandList()
{
    this->command_list_ = GraphicsCommandListFactory::create(this->device_context_->getDevice(), this->command_allocator_->GetAllocator());
}

/// <summary>
/// レンダーターゲット生成
/// </summary>
void DX12Resources::initRenderTarget()
{
    RenderTarget::RenderTargetConf rt_conf = {};
    rt_conf.frame_buffer_count = frameBufferCount;
    rt_conf.swap_chain = this->swap_chain_->getSwapChain();
    this->render_target_ = RenderTargetFactory::create(rt_conf, this->device_context_->getDevice());
}

/// <summary>
/// オフスクリーンレンダーターゲット生成
/// </summary>
void DX12Resources::initCompositeRenderTarget()
{
    CompositeRenderTarget::CompositeRenderTargetConf crt_conf = {};
    crt_conf.resource_desc = this->render_target_->getResource(0)->GetDesc();
    crt_conf.descriptor_heap_desc = this->render_target_->getDescriptorHeap()->GetDesc();
    this->composite_render_target_ = CompositeRenderTargetFactory::create(crt_conf, this->device_context_->getDevice());
}

/// <summary>
/// 深度ステンシル生成
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
void DX12Resources::initDepthStencil()
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.width = windowWidth;
    ds_conf.height = windowHeight;
    ds_conf.frame_buffer_count = frameBufferCount;
    this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, this->device_context_->getDevice());
}

/// <summary>
/// GPUとCPUの同期オブジェクト(fence)生成
/// </summary>
void DX12Resources::initFence()
{
    this->fence_ = FenceFactory::create(this->device_context_->getDevice());
}

/// <summary>
/// ビューポートを設定する
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
void DX12Resources::initViewport()
{
    this->viewport_.TopLeftX = 0;
    this->viewport_.TopLeftY = 0;
    this->viewport_.Width = static_cast<FLOAT>(windowWidth);
    this->viewport_.Height = static_cast<FLOAT>(windowHeight);
    this->viewport_.MinDepth = D3D12_MIN_DEPTH;
    this->viewport_.MaxDepth = D3D12_MAX_DEPTH;
}

/// <summary>
/// シザリング矩形を設定する
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
void DX12Resources::initScissorRect()
{
    this->scissor_rect_.left = 0;
    this->scissor_rect_.top = 0;
    this->scissor_rect_.right = windowWidth;
    this->scissor_rect_.bottom = windowHeight;
}

/// <summary>
/// レンダーコンテキストの生成
/// </summary>
void DX12Resources::initRenderContext()
{
    this->render_context_ = RenderContextFactory::create(this->command_list_->GetCommandList(), this->viewport_);
}

/// <summary>
/// フルスクリーン四角形を生成する
/// </summary>
void DX12Resources::initFullScreenQuad()
{
    this->full_screen_quad_ = FullScreenQuadFactory::create(this->device_context_->getDevice());
}

/// <summary>
/// RTVハンドルを設定する
/// </summary>
void DX12Resources::setMainRTVHandle()
{
    //レンダーターゲットのディスクリプタヒープの先頭アドレスを取得
    this->current_frame_buffer_rtv_handle_ = this->render_target_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    //フレームバッファ数分ずらす
    this->current_frame_buffer_rtv_handle_.ptr += static_cast<unsigned long long>(this->render_target_->getDescriptorHeapSize()) * this->frame_index_;
}

/// <summary>
/// オフスクリーン用のRTVハンドルを設定する
/// </summary>
void DX12Resources::setOffScreenRTVHandle()
{
    this->current_frame_buffer_rtv_handle_ = this->composite_render_target_->getRTVHeap()->GetCPUDescriptorHandleForHeapStart();
}

/// <summary>
/// DSVハンドルを設定する
/// </summary>
void DX12Resources::updateDSVHandle()
{
    this->current_frame_buffer_dsv_handle_ = this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
}