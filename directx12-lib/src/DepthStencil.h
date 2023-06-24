#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
using namespace Microsoft::WRL;

struct DepthStencilConf
{
    ID3D12Device* device;
    UINT frameBufferCount;
    UINT width;
    UINT height;
};


class DepthStencil
{
public:
    DepthStencil() {};
    ~DepthStencil() {};

    void init(const DepthStencilConf depthStencilConf);

private:
    //ディスクリプタヒープの生成
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const DepthStencilConf depthStencilConf);

    //ディスクリプタヒープのサイズの取得
    //int createDescriptorHeapSize(const DepthStencilConf depthStencilConf);

    //リソースの生成
    void createResource(const DepthStencilConf depthStencilConf);

public:
    ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptorHeap.Get(); } //ディスクリプタヒープの取得
    //int getDescriptorHeapSize() const { return descriptorHeapSize; } //ディスクリプタヒープのサイズの取得
    ID3D12Resource* getResource() const { return resource.Get(); } //リソースの取得
private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //ディスクリプタヒープ
    //int descriptorHeapSize;                         //ディスクリプタヒープのサイズ
    ComPtr<ID3D12Resource> resource;   //リソース
};
