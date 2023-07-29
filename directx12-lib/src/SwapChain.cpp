#include "SwapChain.h"
#include <stdexcept>

/// <summary>
/// 初期化
/// </summary>
void SwapChain::init()
{
    createSwapChain();
    createCurrentBackBufferIndex();
}

/// <summary>
/// バックバッファとフロントバッファを入れ替える
/// </summary>
void SwapChain::present()
{
    //スワップチェインのプレゼント
    if (FAILED(this->swap_chain_->Present(1, 0))) {
        throw std::runtime_error("スワップチェインのプレゼントに失敗しました");
    }

    // バックバッファインデックスの更新
    this->current_back_buffer_index_ = this->swap_chain_->GetCurrentBackBufferIndex();
}

/// <summary>
/// スワップチェイン生成
/// </summary>
void SwapChain::createSwapChain()
{
    //スワップチェインの設定
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = this->conf_.frame_buffer_count;
    swapChainDesc.Width = this->conf_.width;
    swapChainDesc.Height = this->conf_.height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    //スワップチェイン1生成
    ComPtr<IDXGISwapChain1> swap_chain1;
    if (FAILED(this->conf_.factory->CreateSwapChainForHwnd(
        this->conf_.command_queue,
        this->conf_.hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swap_chain1
    ))) {
        throw std::runtime_error("スワップチェイン1の生成に失敗しました");
    }

    //IDXGISwapChain3のインターフェースを取得
    if (FAILED(swap_chain1->QueryInterface(IID_PPV_ARGS(&this->swap_chain_)))) {
        throw std::runtime_error("スワップチェイン3のインターフェースの取得に失敗しました");
    }
}

/// <summary>
/// 現在のバックバッファのインデックスを取得
/// </summary>
void SwapChain::createCurrentBackBufferIndex()
{
    this->current_back_buffer_index_ = this->swap_chain_->GetCurrentBackBufferIndex();
}