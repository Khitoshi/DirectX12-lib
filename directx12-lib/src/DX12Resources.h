#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h"
#include <dxgi1_4.h>

#include "SwapChain.h"
#include "RenderTarget.h"
#include "OffScreenRenderTarget.h"
#include "DepthStencil.h"
#include "RenderContext.h"
#include "Fence.h"
#include "RootSignature.h"
#include "FullScreenQuad.h"
using namespace Microsoft::WRL;

/// <summary>
/// DirectX12のリソースを纏めて処理するクラス
/// </summary>
class DX12Resources
{
public:
    DX12Resources() :
        device(nullptr),
        commandQueue(),
        swapChain(),
        commandAllocator(),
        commandList(),
        renderTarget(),
        offScreenRenderTarget(),
        depthStencil(),
        fence(),
        renderContext(),
        viewport(),
        scissorRect(),
        currentFrameBufferRTVHandle(),
        currentFrameBufferDSVHandle(),
        frameIndex(0),
        fullScreenQuad(),
        backGroundColor()
    {}
    ~DX12Resources() { waitEndOfDrawing(); }

    //初期化処理
    void init(const HWND hWnd, const int width, const int height, const int frameBufferCount);

    //レンダリング開始処理
    void beginRender();

    //レンダリング終了処理
    void endRender();

    //描画の終了を待機する
    void waitEndOfDrawing();

private://生成系
    //IDXGIファクトリ生成
    ComPtr<IDXGIFactory4> createFactory();
    //デバイス生成
    ComPtr<ID3D12Device5> createDevice(IDXGIFactory4* dxgiFactory);
    //コマンドキュー生成
    ComPtr<ID3D12CommandQueue> createCommandQueue();
    //スワップチェイン生成
    std::shared_ptr<SwapChain> createSwapChain(const HWND hWnd, const int width, const int height, const int frameBufferCount, IDXGIFactory4* factory);
    //コマンドアロケータ生成
    ComPtr<ID3D12CommandAllocator> createCommandAllocator();
    //コマンドリスト生成
    ComPtr<ID3D12GraphicsCommandList4> createCommandList();
    //Mainレンダーターゲット生成
    std::shared_ptr<RenderTarget> createRenderTarget(const int width, const int height, const UINT frameBufferCount);
    //オフスクリーンレンダーターゲット生成
    std::shared_ptr<OffScreenRenderTarget> createOffScreenRenderTarget();
    //深度ステンシル生成
    std::shared_ptr<DepthStencil> createDepthStencil(const int width, const int height, const UINT frameBufferCount);
    //フェンス生成
    std::shared_ptr<Fence> createFence();
    //レンダーコンテキスト生成
    std::shared_ptr<RenderContext> createRenderContext();

    //ビューポート設定
    D3D12_VIEWPORT createViewport(const int width, const int height);
    //シザリング矩形設定
    D3D12_RECT createScissorRect(const int width, const int height);

    //レンダーターゲットビューのハンドルを設定
    void setMainRTVHandle();
    void setOffScreenRTVHandle();

    //深度ステンシルビューのハンドルを設定
    void setDSVHandle();

public://設定系
    void setBackGroundColor(const float color[4])
    {
        this->backGroundColor[0] = color[0];
        this->backGroundColor[1] = color[1];
        this->backGroundColor[2] = color[2];
        this->backGroundColor[3] = color[3];
    };
public://取得系
    //デバイス取得
    ID3D12Device5* getDevice() const { return device.Get(); }
    //レンダーコンテキスト取得
    RenderContext* getRenderContext() const { return renderContext.get(); }
private:
    ComPtr<ID3D12Device5>device;                                    //デバイス
    ComPtr<ID3D12CommandQueue>commandQueue;                         //コマンドキュー
    std::shared_ptr<SwapChain>swapChain;                            //スワップチェイン
    ComPtr<ID3D12CommandAllocator>commandAllocator;                 //コマンドアロケータ
    ComPtr<ID3D12GraphicsCommandList4>commandList;                  //コマンドリスト
    std::shared_ptr<RenderTarget>renderTarget;                      //レンダーターゲット
    std::shared_ptr<OffScreenRenderTarget>offScreenRenderTarget;    //オフスクリーンレンダーターゲット
    std::shared_ptr<DepthStencil>depthStencil;                      //深度ステンシル

    std::shared_ptr<Fence> fence;                                   //フェンス
    std::shared_ptr<RenderContext> renderContext;                   //レンダーコンテキスト

    D3D12_VIEWPORT viewport;                                        //ビューポート
    D3D12_RECT scissorRect;                                         //シザリング矩形

    D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle;		//現在書き込み中のフレームバッファのレンダーターゲットビューのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferDSVHandle;		//現在書き込み中のフレームバッファの深度ステンシルビューのハンドル

    int frameIndex;                                                 //フレームの番号
    std::shared_ptr<FullScreenQuad> fullScreenQuad;                 //フルスクリーン四角形

    float backGroundColor[4];                                       //背景色
};
