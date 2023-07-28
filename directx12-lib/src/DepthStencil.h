#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
using namespace Microsoft::WRL;

/// <summary>
/// 深度ステンシルバッファ生成用クラス
/// </summary>
class DepthStencil
{
    friend class DepthStencilFactory;
public:
    /// <summary>
    /// 深度ステンシルバッファ生成時に使用する設定
    /// </summary>
    struct DepthStencilConf
    {
        UINT frame_buffer_count;    //フレームバッファ数
        UINT width;                 //幅
        UINT height;                //高さ
    };

private:
    DepthStencil(const DepthStencilConf c) :conf_(c), descriptor_heap_(), resource_() {};

public:
    ~DepthStencil() {};

private:
    //初期化処理
    void init(ID3D12Device* device);
    //ディスクリプタヒープの生成
    void createDescriptorHeap(ID3D12Device* device);
    //リソースの生成
    void createResource(ID3D12Device* device);
    //深度ステンシルビュー生成
    void createDSV(ID3D12Device* device);

public:
    //ディスクリプタヒープの取得
    ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptor_heap_.Get(); }

    //リソースの取得
    ID3D12Resource* getResource() const { return resource_.Get(); }

private:
    DepthStencilConf conf_;                             //深度ステンシルバッファ生成時に使用する設定
    ComPtr<ID3D12DescriptorHeap>    descriptor_heap_;   //ディスクリプタヒープ
    ComPtr<ID3D12Resource>          resource_;          //リソース
};
