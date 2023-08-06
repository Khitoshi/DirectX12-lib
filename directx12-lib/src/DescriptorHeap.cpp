#include "DescriptorHeap.h"
#include <stdexcept>

void DescriptorHeap::init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors)
{
    //ディスクリプタヒープの設定
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NodeMask = 0;
    desc.NumDescriptors = num_descriptors;
    desc.Type = type;
    //desc.Flags = ? : ;
    if (desc.Type != D3D12_DESCRIPTOR_HEAP_TYPE_RTV && desc.Type != D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    }
    else {
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    }

    //ディスクリプタヒープの作成
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->descriptor_heap_)))) {
        throw std::runtime_error("failed to create descriptor heap");
    }
}

