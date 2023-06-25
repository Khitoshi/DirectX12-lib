#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
using namespace Microsoft::WRL;

/// <summary>
/// 深度ステンシルバッファ生成時に使用する設定
/// </summary>
struct DepthStencilConf
{
    ID3D12Device* device;
    UINT frameBufferCount;
    UINT width;
    UINT height;
};

/// <summary>
/// 深度ステンシルバッファ生成用クラス
/// </summary>
class DepthStencil
{
public:
    DepthStencil() :descriptorHeap(), resource() {};
    ~DepthStencil() {};

    //初期化処理
    void init(const DepthStencilConf depthStencilConf);

private:
    //ディスクリプタヒープの生成
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const DepthStencilConf depthStencilConf);

    //リソースの生成
    void createResource(const DepthStencilConf depthStencilConf);

public:
    ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptorHeap.Get(); }    //ディスクリプタヒープの取得
    ID3D12Resource* getResource() const { return resource.Get(); }                      //リソースの取得

private:
    ComPtr<ID3D12DescriptorHeap>    descriptorHeap;         //ディスクリプタヒープ
    ComPtr<ID3D12Resource>          resource;               //リソース
};
