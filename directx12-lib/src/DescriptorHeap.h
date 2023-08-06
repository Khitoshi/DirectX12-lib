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
    //����������
    void init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors);
public:
    ID3D12DescriptorHeap* getDescriptorHeap() { return descriptor_heap_.Get(); }
private:
    ComPtr<ID3D12DescriptorHeap> descriptor_heap_;	//�f�B�X�N���v�^�q�[�v
};
