#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

class DescriptorHeap
{
    friend class DescriptorHeapFactory;
private:

    DescriptorHeap() :
        descriptor_heap_()
    {};
public:
    ~DescriptorHeap() {};

private:
    //初期化処理
    void init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors);
public:
    ID3D12DescriptorHeap* getDescriptorHeap() { return descriptor_heap_.Get(); }
private:
    ComPtr<ID3D12DescriptorHeap> descriptor_heap_;	//ディスクリプタヒープ
};
