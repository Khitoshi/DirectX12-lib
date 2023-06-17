#include "SwapChain.h"
#include <stdexcept>

/// <summary>
/// 初期化
/// </summary>
void SwapChain::init(const SwapChainConf swapChainConf)
{
    this->swapChain = createSwapChain(swapChainConf);

    this->currentBackBufferIndex = createCurrentBackBufferIndex();
}

/// <summary>
/// バックバッファとフロントバッファを入れ替える
/// </summary>
void SwapChain::present()
{
    //スワップチェインのプレゼント
    if (FAILED(swapChain->Present(1, 0))) {
        throw std::runtime_error("スワップチェインのプレゼントに失敗しました");
    }

    // バックバッファインデックスの更新
    this->currentBackBufferIndex = this->swapChain->GetCurrentBackBufferIndex();
}

/// <summary>
/// スワップチェイン生成
/// </summary>
/// <returns>生成したスワップチェイン</returns>
ComPtr<IDXGISwapChain3> SwapChain::createSwapChain(const SwapChainConf swapChainConf)
{
    //スワップチェインの設定
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = swapChainConf.frameBufferCount;
    swapChainDesc.Width = swapChainConf.width;
    swapChainDesc.Height = swapChainConf.height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    //フルスクリーン設定
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
    fullScreenDesc.Windowed = TRUE;

    //スワップチェイン1生成
    ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(swapChainConf.factory->CreateSwapChainForHwnd(
        swapChainConf.commandQueue,
        swapChainConf.hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    ))) {
        throw std::runtime_error("スワップチェイン1の生成に失敗しました");
    }

    //IDXGISwapChain3のインターフェースを取得
    ComPtr<IDXGISwapChain3> swapChain3;
    if (FAILED(swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain3)))) {
        throw std::runtime_error("スワップチェイン3のインターフェースの取得に失敗しました");
    }

    return swapChain3;
}

/// <summary>
/// 現在のバックバッファのインデックスを取得
/// </summary>
/// <returns>現在のバックバッファのインデックス</returns>
UINT SwapChain::createCurrentBackBufferIndex()
{
    return this->swapChain->GetCurrentBackBufferIndex();
}