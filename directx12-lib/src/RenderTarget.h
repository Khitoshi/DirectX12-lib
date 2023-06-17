#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
using namespace Microsoft::WRL;

/// <summary>
/// レンダーターゲット生成時に使用する設定
/// </summary>
struct RenderTargetConf {
    int width;
    int height;
    UINT frameBufferCount;
    ID3D12Device* device;
    IDXGISwapChain3* swapChain;
};

class RenderTarget
{
public:
    RenderTarget() :
        descriptorHeap(),
        descriptorHeapSize(0),
        resource()
    {};
    ~RenderTarget() {};

    //初期化処理
    void init(RenderTargetConf renderTargetConf);
private:
    //ディスクリプタヒープの生成
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const RenderTargetConf renderTargetConf);

    //ディスクリプタヒープのサイズの取得
    int createDescriptorHeapSize(const RenderTargetConf renderTargetConf);

    //リソースの生成
    std::vector<ComPtr<ID3D12Resource>> createResource(const RenderTargetConf renderTargetConf);

public:
    ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptorHeap.Get(); } //ディスクリプタヒープの取得
    int getDescriptorHeapSize() const { return descriptorHeapSize; } //ディスクリプタヒープのサイズの取得
    ID3D12Resource* getResource(int index) const { return resource[index].Get(); } //リソースの取得

private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //ディスクリプタヒープ
    int descriptorHeapSize;                         //ディスクリプタヒープのサイズ
    std::vector<ComPtr<ID3D12Resource>> resource;   //リソース
};
