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
    DepthStencil() {};
    ~DepthStencil() {};

    void init(const DepthStencilConf depthStencilConf);

private:
    //ディスクリプタヒープの生成
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const DepthStencilConf depthStencilConf);

    //ディスクリプタヒープのサイズの取得
    int createDescriptorHeapSize(const DepthStencilConf depthStencilConf);

    //リソースの生成
    ComPtr<ID3D12Resource> createResource(const DepthStencilConf depthStencilConf);


private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //ディスクリプタヒープ
    int descriptorHeapSize;                         //ディスクリプタヒープのサイズ
    ComPtr<ID3D12Resource> resource;   //リソース
};
