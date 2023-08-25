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

    void resourceReset(const UINT i) { resource_[i].Reset(); }

    void createResource(ID3D12Device* device);
private:
    void init(ID3D12Device* device);
    void createDescriptorHeap(ID3D12Device* device);
    void createDescriptorHeapSize(ID3D12Device* device);

public://取得系
    ID3D12DescriptorHeap* getDescriptorHeap() const;
    int getDescriptorHeapSize() const { return this->descriptor_heap_size_; }
    ID3D12Resource* getResource(int i) const { return resource_[i].Get(); }

private:
    RenderTargetConf conf_;
    std::shared_ptr<DescriptorHeap> descriptor_heap_;

    int descriptor_heap_size_;
    std::vector<ComPtr<ID3D12Resource>> resource_;
};
