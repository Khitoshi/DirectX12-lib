#include "ConstantBuffer.h"
#include <stdexcept>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">GPUデバイス</param>
void ConstantBuffer::init(ID3D12Device* device)
{
    this->createResource(device);
    this->createDescriptorHeap(device);
    this->createView(device);
}

/// <summary>
/// リソース生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void ConstantBuffer::createResource(ID3D12Device* device)
{
    auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buf = CD3DX12_RESOURCE_DESC::Buffer((sizeof(DirectX::XMMATRIX) + 0xff) & ~0xff);

    device->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buf,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&this->resource_));
    if (FAILED(this->resource_->SetName(L"Constant Buffer Upload Resource Heap"))) {
        throw std::runtime_error("ConstantBuffer::createResource() : resource->SetName() Failed.");
    }
}

/// <summary>
/// ディスクリプタヒープ生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void ConstantBuffer::createDescriptorHeap(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors = 1;
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (FAILED(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&this->descriptor_heap_)))) {
        throw std::runtime_error("ConstantBuffer::createDescriptorHeap() : device->CreateDescriptorHeap() Failed.");
    }

    if (FAILED(this->descriptor_heap_->SetName(L"Constant Buffer Descriptor Heap"))) {
        throw std::runtime_error("ConstantBuffer::createDescriptorHeap() : descriptorHeap->SetName() Failed.");
    }
}

/// <summary>
/// ディスクリプタヒープビュー生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void ConstantBuffer::createView(ID3D12Device* device)
{
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
    cbv_desc.BufferLocation = this->resource_->GetGPUVirtualAddress();
    cbv_desc.SizeInBytes = (sizeof(DirectX::XMMATRIX) + 0xff) & ~0xff;
    device->CreateConstantBufferView(&cbv_desc, this->descriptor_heap_->GetCPUDescriptorHandleForHeapStart());
    if (FAILED(this->resource_->SetName(L"Constant Buffer View"))) {
        throw std::runtime_error("ConstantBuffer::createView() : resource->SetName() Failed.");
    }
}