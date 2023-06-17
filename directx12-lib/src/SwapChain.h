#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
using namespace Microsoft::WRL;

//スワップチェインの生成時に使用する設定
struct SwapChainConf {
    UINT frameBufferCount;           //バッファ数
    UINT width;                      //幅
    UINT height;                     //高さ
    HWND hWnd;                       //ウィンドウハンドル
    IDXGIFactory4* factory;      //DXGIファクトリ
    ID3D12CommandQueue* commandQueue;//コマンドキュー
};

/// <summary>
/// スワップチェイン
/// </summary>
class SwapChain
{
public:

public:
    SwapChain() :
        swapChain(),
        currentBackBufferIndex(0)
    {};
    ~SwapChain() {};

    //スワップチェイン関係の生成
    void init(const SwapChainConf swapChainConf);

    //スワップチェインのプレゼント
    void present();

private:
    //スワップチェインの生成
    ComPtr<IDXGISwapChain3> createSwapChain(const SwapChainConf swapChainConf);
    //バックバッファの生成
    UINT createCurrentBackBufferIndex();

public:
    IDXGISwapChain3* getSwapChain() const { return swapChain.Get(); } //スワップチェインの取得
    UINT getCurrentBackBufferIndex() const { return currentBackBufferIndex; } //現在のバックバッファの番号の取得
private:
    ComPtr<IDXGISwapChain3> swapChain;      //スワップチェイン
    UINT currentBackBufferIndex;            //現在のバックバッファの番号
};
