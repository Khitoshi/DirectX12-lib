#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>

class DescriptorHeap;

using namespace Microsoft::WRL;

/// <summary>
/// レンダーターゲット生成用クラス
/// </summary>
class RenderTarget
{
    friend class RenderTargetFactory;
public:
    /// <summary>
    /// レンダーターゲット生成時に使用する設定
    /// </summary>
    struct RenderTargetConf {
        UINT frame_buffer_count;        //フレームバッファの数
        IDXGISwapChain3* swap_chain;    //スワップチェイン
    };

private:
    RenderTarget(const RenderTargetConf c) :
        conf_(c),
        descriptor_heap_(),
        descriptor_heap_size_(0),
        resource_()
    {};

public:
    ~RenderTarget() {};

private:
    //初期化処理
    void init(ID3D12Device* device);
    //ディスクリプタヒープの生成
    void createDescriptorHeap(ID3D12Device* device);
    //ディスクリプタヒープのサイズの取得
    void createDescriptorHeapSize(ID3D12Device* device);
    //リソースの生成
    void createResource(ID3D12Device* device);

public:
    //ディスクリプタヒープの取得
    ID3D12DescriptorHeap* getDescriptorHeap() const;
    //ディスクリプタヒープのサイズの取得
    int getDescriptorHeapSize() const { return this->descriptor_heap_size_; }
    //リソースの取得
    ID3D12Resource* getResource(int index) const { return resource_[index].Get(); }

private:
    RenderTargetConf conf_;                         //設定

    //ComPtr<ID3D12DescriptorHeap> descriptor_heap_;  //ディスクリプタヒープ
    std::shared_ptr<DescriptorHeap> descriptor_heap_;  //ディスクリプタヒープ

    int descriptor_heap_size_;                      //ディスクリプタヒープのサイズ
    std::vector<ComPtr<ID3D12Resource>> resource_;  //リソース
};
