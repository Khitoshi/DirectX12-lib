#include "DX12Resources.h"
#include "OffScreenRenderTargetCacheManager.h"
#include <stdexcept>
void DX12Resources::init(const HWND hWnd, const int width, const int height, const int frameBufferCount)
{
    //所有権が変わるので解放は合計で1回になる
    ComPtr<IDXGIFactory4> factory = createFactory();
    this->device = createDevice(factory.Get());
    this->commandQueue = createCommandQueue();
    this->swapChain = createSwapChain(hWnd, width, height, frameBufferCount, factory.Get());
    this->commandAllocator = createCommandAllocator();
    this->commandList = createCommandList();
    this->renderTarget = createRenderTarget(width, height, frameBufferCount);
    this->compositeRenderTarget = createCompositeRenderTarget();
    this->depthStencil = createDepthStencil(width, height, frameBufferCount);
    this->fence = createFence();
    this->renderContext = createRenderContext();
    this->viewport = createViewport(width, height);
    this->scissorRect = createScissorRect(width, height);

    fullScreenQuad = std::make_unique<FullScreenQuad>();
    fullScreenQuad->init(this->device.Get());



    //オフスクリーンで使用する深度ステンシルビューのハンドルを設定
    //OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferRTVHandle);
    OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferDSVHandle);
    //オフスクリーンで使用するビューポートを設定
    OffScreenRenderTargetCacheManager::getInstance().setViewport(this->viewport);

    //TODO:リファクタリング対象
    OffScreenRenderTarget::OffScreenRenderTargetConf conf = {};
    conf.clearColor[0] = this->backGroundColor[0];
    conf.clearColor[1] = this->backGroundColor[1];
    conf.clearColor[2] = this->backGroundColor[2];
    conf.clearColor[3] = this->backGroundColor[3];
    conf.descriptorHeapDesc = this->renderTarget->getDescriptorHeap()->GetDesc();
    conf.resourceDesc = this->renderTarget->getResource(this->frameIndex)->GetDesc();
    OffScreenRenderTargetCacheManager::getInstance().setConf(conf);
}

/// <summary>
/// レンダリング開始処理
/// </summary>
/// <param name="color">clear color</param>
void DX12Resources::beginRender()
{
    //バックバッファインデックスの取得
    this->frameIndex = this->swapChain->getCurrentBackBufferIndex();

    //コマンドアロケータのリセット
    this->commandAllocator->Reset();

    //コマンドリストのリセット
    this->renderContext->reset(this->commandAllocator.Get(), nullptr);

    //深度ステンシルビューのハンドルを設定
    this->setDSVHandle();

    //オフスクリーンで使用する深度ステンシルビューのハンドルを設定
    //OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferRTVHandle);
    OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferDSVHandle);
    //オフスクリーンで使用するビューポートを設定
    OffScreenRenderTargetCacheManager::getInstance().setViewport(this->viewport);

    //TODO:リファクタリング対象
    OffScreenRenderTarget::OffScreenRenderTargetConf conf = {};
    conf.clearColor[0] = this->backGroundColor[0];
    conf.clearColor[1] = this->backGroundColor[1];
    conf.clearColor[2] = this->backGroundColor[2];
    conf.clearColor[3] = this->backGroundColor[3];
    conf.descriptorHeapDesc = this->renderTarget->getDescriptorHeap()->GetDesc();
    conf.resourceDesc = this->renderTarget->getResource(this->frameIndex)->GetDesc();
    OffScreenRenderTargetCacheManager::getInstance().setConf(conf);
}

/// <summary>
/// レンダリング終了処理
/// </summary>
void DX12Resources::endRender()
{
    //offscreenを1枚のテクスチャにまとめる描画開始処理
    this->compositeRenderTarget->beginRender(this->renderContext.get(), this->currentFrameBufferDSVHandle);
    //offscreenを1枚のテクスチャにまとめる描画処理
    this->compositeRenderTarget->render(this->renderContext.get(), this->device.Get());
    //offscreenを1枚のテクスチャにまとめる描画終了処理
    this->compositeRenderTarget->endRender(this->renderContext.get());

    //Mainレンダーターゲットの描画開始

    //レンダーターゲットの設定
    this->setMainRTVHandle();

    //ビューポートとシザリング矩形の設定
    this->renderContext->transitionMainRenderTargetBegin(this->renderTarget->getResource(this->frameIndex));
    this->renderContext->executeResourceBarriers();
    this->renderContext->setRenderTarget(this->currentFrameBufferRTVHandle, this->currentFrameBufferDSVHandle);
    this->renderContext->setViewport(this->viewport);
    this->renderContext->setScissorRect(this->viewport);

    //深度ステンシルのクリア
    this->renderContext->clearRenderTarget(this->currentFrameBufferRTVHandle, this->backGroundColor);
    this->renderContext->clearDepthStencil(this->currentFrameBufferDSVHandle, 1.0f);

    //offscreenをテクスチャとしたフルスクリーン四角形を描画
    fullScreenQuad->draw(this->renderContext.get(), this->device.Get(), compositeRenderTarget.get());

    //Mainレンダーターゲットの描画完了を待つ
    this->renderContext->transitionMainRenderTargetEnd(this->renderTarget->getResource(this->frameIndex));

    //リソースバリアの実行
    this->renderContext->executeResourceBarriers();

    //コマンドのクローズ
    this->renderContext->close();

    //コマンドの実行
    ID3D12CommandList* cmd_list[] = { this->commandList.Get() };
    this->commandQueue->ExecuteCommandLists(_countof(cmd_list), cmd_list);

    //スワップチェインの更新
    this->swapChain->present();

    //描画完了を待つ
    waitEndOfDrawing();
}

/// <summary>
/// 描画の終了を待つ
/// </summary>
void DX12Resources::waitEndOfDrawing()
{
    //フェンスのシグナル
    this->commandQueue->Signal(this->fence->getFence(), this->fence->getValue());

    //フェンスの待機
    if (this->fence->getFence()->GetCompletedValue() < this->fence->getValue()) {
        this->fence->getFence()->SetEventOnCompletion(this->fence->getValue(), this->fence->getEvent());
        WaitForSingleObject(this->fence->getEvent(), INFINITE);
    }

    //フェンスの値を更新
    this->fence->incrementValue();
}

/// <summary>
/// ファクトリ　生成
/// </summary>
/// <returns>
/// 生成したファクトリのポイント
/// </returns>
ComPtr<IDXGIFactory4> DX12Resources::createFactory() {
    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    //デバッグコントローラーがあれば、デバッグレイヤーがあるDXGIを作成する。
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();

        //デバッグの場合デバッグフラグを立てる
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#if 0 // GBV を有効化する場合.
        ComPtr<ID3D12Debug3> debug3;
        debug.As(&debug3);
        if (debug3)
        {
            debug3->SetEnableGPUBasedValidation(true);
        }
#endif
    }
    else {
        throw std::runtime_error("failed to create debug Controller");
    }

#endif

    ComPtr<IDXGIFactory4> factory;

    //factory生成
    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("failed to create dxgi factory");
    }

    return factory;
}

/// <summary>
/// デバイス生成
/// </summary>
/// <param name="dxgiFactory">IDXGIfactory</param>
/// <returns>
///	生成したデバイスのポインタ
/// </returns>
ComPtr<ID3D12Device5> DX12Resources::createDevice(IDXGIFactory4* dxgiFactory)
{
    //GPUベンダー定義
    enum class GPU_Vender {
        NVIDIA,	//NVIDIA
        AMD,	//Intel
        INTEL,	//AMD

        NUM,	//Vender数
    };

    IDXGIAdapter* adapterTmp = nullptr;
    IDXGIAdapter* adapterVender[(int)GPU_Vender::NUM] = { nullptr };//各ベンダーのアダプター
    IDXGIAdapter* adapterMaxVideoMemory = nullptr;					//最大ビデオメモリのアダプタ
    IDXGIAdapter* useAdapter = nullptr;								//最終的に使用するアダプタ
    SIZE_T videoMemorySize = 0;										//最大ビデオメモリのサイズ

    for (int i = 0; dxgiFactory->EnumAdapters(i, &adapterTmp) != DXGI_ERROR_NOT_FOUND; i++) {
        //アダプターの情報を取得。
        DXGI_ADAPTER_DESC desc;
        adapterTmp->GetDesc(&desc);

        //最大ビデオメモリのアダプターを保存。
        //ここで取得したアダプタはAMDやINTEL等のGPUがない場合に使用するアダプタ
        if (desc.DedicatedVideoMemory > videoMemorySize) {
            //すでに登録している場合は、解放してから登録。
            if (adapterMaxVideoMemory != nullptr) {
                adapterMaxVideoMemory->Release();
            }
            adapterMaxVideoMemory = adapterTmp;
            adapterMaxVideoMemory->AddRef();
            videoMemorySize = desc.DedicatedVideoMemory;
        }

        //ベンダー別にアダプターを保存。
        //すでに登録している場合は、解放してから登録
        if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {//NVIDIA
            if (adapterVender[(int)GPU_Vender::NVIDIA]) {
                adapterVender[(int)GPU_Vender::NVIDIA]->Release();
            }
            adapterVender[(int)GPU_Vender::NVIDIA] = adapterTmp;
            adapterVender[(int)GPU_Vender::NVIDIA]->AddRef();
        }
        else if (wcsstr(desc.Description, L"AMD") != nullptr) {//AMD
            if (adapterVender[(int)GPU_Vender::AMD]) {
                adapterVender[(int)GPU_Vender::AMD]->Release();
            }
            adapterVender[(int)GPU_Vender::AMD] = adapterTmp;
            adapterVender[(int)GPU_Vender::AMD]->AddRef();
        }
        else if (wcsstr(desc.Description, L"Intel") != nullptr) {//Intel
            if (adapterVender[(int)GPU_Vender::INTEL]) {
                adapterVender[(int)GPU_Vender::INTEL]->Release();
            }
            adapterVender[(int)GPU_Vender::INTEL] = adapterTmp;
            adapterVender[(int)GPU_Vender::INTEL]->AddRef();
        }

        //アダプターを解放
        adapterTmp->Release();
    }

    //使用するアダプタを決める(現在はNVIDIAが最優先)
    // NVIDIA >> AMD >> intel >> other
    if (adapterVender[(int)GPU_Vender::NVIDIA] != nullptr) useAdapter = adapterVender[(int)GPU_Vender::NVIDIA];
    else if (adapterVender[(int)GPU_Vender::AMD] != nullptr) useAdapter = adapterVender[(int)GPU_Vender::AMD];
    else if (adapterVender[(int)GPU_Vender::INTEL] != nullptr) useAdapter = adapterVender[(int)GPU_Vender::INTEL];
    else useAdapter = adapterMaxVideoMemory;

    //使用している可能性のあるFEATURE_LEVELを列挙
    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    ComPtr<ID3D12Device5> device;
    //D3D_FEATURE_LEVELのレベルの高い順にloop
    //HACK:https://forums.developer.nvidia.com/t/poco-notfoundexception-thrown-in-driver-version-531-18/245285
    //Poco::NotFoundExceptionの例外スローが発生中，原因はNVIDIAのドライバ
    for (auto featureLevel : featureLevels) {
        if (SUCCEEDED(D3D12CreateDevice(
            useAdapter,
            featureLevel,
            IID_PPV_ARGS(&device)
        ))) {//生成に成功
            break;
        }
    }

    //取得した全てのグラフィックカードを解放する
    for (auto& adapter : adapterVender) {
        if (adapter) {//存在する場合解放
            adapter->Release();
        }
    }

    //GPU以外でグラフィック能力のあるデバイス解放
    if (adapterMaxVideoMemory) {
        adapterMaxVideoMemory->Release();
    }

    //デバイスが生成できなかった場合エラー
    if (device == nullptr) {
        throw std::runtime_error("failed to create device");
    }

    return device;
}

/// <summary>
/// コマンドキューの生成
/// </summary>
/// <returns>生成したコマンドキューのポインタ</returns>
ComPtr<ID3D12CommandQueue> DX12Resources::createCommandQueue()
{
    //コマンドキューの設定
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.NodeMask = 0;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    //コマンドキュー生成
    ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
    if (FAILED(this->device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)))) {
        throw std::runtime_error("failed to create command queue");
    }

    return commandQueue;
}

/// <summary>
/// スワップチェイン生成
/// </summary>
/// <param name="hWnd">ウィンドウハンドル</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="factory">dxgiファクトリ</param>
/// <returns>
/// 生成したスワップチェイン
/// </returns>
std::shared_ptr<SwapChain> DX12Resources::createSwapChain(const HWND hWnd, const int width, const int height, const int frameBufferCount, IDXGIFactory4* factory) {
    std::shared_ptr<SwapChain> swapChain(new SwapChain());

    SwapChainConf conf = {};
    conf.hWnd = hWnd;
    conf.width = width;
    conf.height = height;
    conf.frameBufferCount = frameBufferCount;
    conf.factory = factory;
    conf.commandQueue = this->commandQueue.Get();
    swapChain->init(conf);

    return swapChain;
}

/// <summary>
/// コマンドアロケータ生成
/// </summary>
/// <returns>
/// 生成したコマンドアロケータ
/// </returns>
ComPtr<ID3D12CommandAllocator> DX12Resources::createCommandAllocator()
{
    //コマンドアロケータ生成
    ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
    if (FAILED(this->device.Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) {
        throw std::runtime_error("failed to create command allocator");
    }

    return commandAllocator;
}

/// <summary>
/// コマンドリスト生成
/// </summary>
/// <returns>
/// 生成したコマンドリスト
/// </returns>
ComPtr<ID3D12GraphicsCommandList4> DX12Resources::createCommandList()
{
    ComPtr<ID3D12GraphicsCommandList4> commandList = nullptr;
    //TODO:現在のGPUが1つなのでnodemaskは0にしているから，複数GPUに対応する場合は変更する必要がある
    //コマンドリスト生成
    if (FAILED(this->device.Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) {
        throw std::runtime_error("failed to create command list");
    }

    //コマンドリスト生成時にはまだコマンドリストはクローズされていないので，クローズする
    if (FAILED(commandList->Close())) {
        throw std::runtime_error("failed to close command list");
    }

    return commandList;
}

/// <summary>
/// レンダーターゲット生成
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="frameBufferCount">フレームバッファ数</param>
/// <returns>
/// 生成&初期化したレンダーターゲット
/// </returns>
std::shared_ptr<RenderTarget> DX12Resources::createRenderTarget(const int width, const int height, const UINT frameBufferCount)
{
    RenderTargetConf rtc = {};
    rtc.device = this->device.Get();
    rtc.frameBufferCount = frameBufferCount;
    rtc.width = width;
    rtc.height = height;
    rtc.swapChain = this->swapChain->getSwapChain();

    std::shared_ptr<RenderTarget> renderTarget(new RenderTarget());
    renderTarget->init(rtc);
    return renderTarget;
}

/// <summary>
/// オフスクリーンレンダーターゲット生成
/// </summary>
/// <returns>
/// 生成&初期化したオフスクリーンレンダーターゲット
/// </returns>
std::shared_ptr<CompositeRenderTarget> DX12Resources::createCompositeRenderTarget()
{
    CompositeRenderTarget::CompositeRenderTargetConf conf = {};
    conf.resource_desc = this->renderTarget->getResource(0)->GetDesc();
    conf.descriptor_heap_desc = this->renderTarget->getDescriptorHeap()->GetDesc();
    conf.clear_color[0] = this->backGroundColor[0];
    conf.clear_color[1] = this->backGroundColor[1];
    conf.clear_color[2] = this->backGroundColor[2];
    conf.clear_color[3] = this->backGroundColor[3];
    std::shared_ptr<CompositeRenderTarget> crt = {};
    crt = std::make_shared<CompositeRenderTarget>(conf);
    crt->init(this->device.Get());
    return crt;
}

/// <summary>
/// 深度ステンシル生成
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="frameBufferCount">バッファ番号</param>
/// <returns>
/// 生成&初期化した深度ステンシル
/// </returns>
std::shared_ptr<DepthStencil> DX12Resources::createDepthStencil(const int width, const int height, const UINT frameBufferCount)
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.width = width;
    ds_conf.height = height;
    ds_conf.frame_buffer_count = frameBufferCount;

    std::shared_ptr<DepthStencil> depthStencil = std::make_shared<DepthStencil>(ds_conf);
    depthStencil->init(this->device.Get());
    return depthStencil;
}

/// <summary>
/// GPUとCPUの同期オブジェクト(fence)生成
/// </summary>
/// <returns>
/// 生成&初期化したfence
/// </returns>
std::shared_ptr<Fence> DX12Resources::createFence()
{
    std::shared_ptr<Fence>fence = std::make_shared<Fence>();
    fence->init(device.Get());
    return fence;
}

/// <summary>
/// レンダーコンテキストの生成
/// </summary>
/// <returns>
/// 生成&初期化したレンダーコンテキスト
/// </returns>
std::shared_ptr<RenderContext> DX12Resources::createRenderContext()
{
    std::shared_ptr<RenderContext> rc = std::make_shared<RenderContext>();
    rc->init(this->commandList.Get());
    return rc;
}

/// <summary>
/// ビューポートを設定する
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <returns>
/// 設定後のビューポート
/// </returns>
D3D12_VIEWPORT DX12Resources::createViewport(const int width, const int height)
{
    D3D12_VIEWPORT vp = {};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = D3D12_MIN_DEPTH;
    vp.MaxDepth = D3D12_MAX_DEPTH;
    return vp;
}

/// <summary>
/// シザリング矩形を設定する
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <returns>
/// 設定済みのシザリング矩形
/// </returns>
D3D12_RECT DX12Resources::createScissorRect(const int width, const int height)
{
    D3D12_RECT rect = {};
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    return rect;
}

/// <summary>
/// RTVハンドルを設定する
/// </summary>
void DX12Resources::setMainRTVHandle()
{
    //レンダーターゲットのディスクリプタヒープの先頭アドレスを取得
    this->currentFrameBufferRTVHandle = this->renderTarget->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    //フレームバッファ数分ずらす
    this->currentFrameBufferRTVHandle.ptr += static_cast<unsigned long long>(this->renderTarget->getDescriptorHeapSize()) * this->frameIndex;
}

void DX12Resources::setOffScreenRTVHandle()
{
    this->currentFrameBufferRTVHandle = this->compositeRenderTarget->getRTVHeap()->GetCPUDescriptorHandleForHeapStart();
    //this->currentFrameBufferRTVHandle.ptr += static_cast<unsigned long long>(this->offScreenRenderTarget->getDescriptorHeapSize()) * this->frameIndex;
}

/// <summary>
/// DSVハンドルを設定する
/// </summary>
void DX12Resources::setDSVHandle()
{
    this->currentFrameBufferDSVHandle = this->depthStencil->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
}

