#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <stdexcept>

using namespace Microsoft::WRL;

/// <summary>
/// スワップチェイン
/// </summary>
class SwapChain
{
    friend class SwapChainFactory;
public:
    //スワップチェインの生成時に使用する設定
    struct SwapChainConf {
        UINT frame_buffer_count;           //バッファ数
        UINT width;                      //幅
        UINT height;                     //高さ
        HWND hWnd;                       //ウィンドウハンドル
        IDXGIFactory4* factory;      //DXGIファクトリ
        ID3D12CommandQueue* command_queue;//コマンドキュー
    };

private:
    SwapChain(const SwapChainConf& c) :
        conf_(c),
        swap_chain_(),
        current_back_buffer_index_(0)
    {};

public:
    ~SwapChain() {};

    //スワップチェインのプレゼント
    void present();

    void resizeBuffer(const UINT& width, const UINT& height);

private:
    //スワップチェイン関係の生成
    void init();
    //スワップチェインの生成
    void createSwapChain();
    //バックバッファの生成
    void createCurrentBackBufferIndex();

public:
    IDXGISwapChain3* getSwapChain() const { return swap_chain_.Get(); }
    UINT getCurrentBackBufferIndex() const { return current_back_buffer_index_; }
    bool isFullScreen() const {
        BOOL fullscreen_state;
        if (FAILED(this->swap_chain_->GetFullscreenState(&fullscreen_state, nullptr))) {
            throw std::runtime_error("フルスクリーン状態の取得に失敗しました");
        }
        return fullscreen_state;
    }
private:
    SwapChainConf conf_;                    //設定
    ComPtr<IDXGISwapChain3> swap_chain_;    //スワップチェイン
    UINT current_back_buffer_index_;        //現在のバックバッファの番号
};
