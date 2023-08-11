#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include <stdexcept>
/// <summary>
/// レジスタに行列情報をコピーする
/// </summary>
/// <param name="src"></param>
void ConstantBuffer::copy(void* src_constants)
{
    int16_t* pData = nullptr;
    if (FAILED(this->resource_->Map(0, nullptr, (void**)&pData))) {
        throw std::exception("ConstantBuffer::copy() : resource->Map() Failed.");
    }
    memcpy(pData, src_constants, this->conf_.size);
    this->resource_->Unmap(0, nullptr);
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">GPUデバイス</param>
void ConstantBuffer::init(ID3D12Device* device)
{
    this->createResource(device);
    this->createView(device);
}

/// <summary>
/// リソース生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void ConstantBuffer::createResource(ID3D12Device* device)
{
    auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buf = CD3DX12_RESOURCE_DESC::Buffer(this->conf_.size + 0xff & ~0xff);

    if (FAILED(device->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buf,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&this->resource_)))) {
        throw std::runtime_error("ConstantBuffer::createResource() : device->CreateCommittedResource() Failed.");
    }


    if (FAILED(this->resource_->SetName(L"Constant Buffer Upload Resource Heap"))) {
        throw std::runtime_error("ConstantBuffer::createResource() : resource->SetName() Failed.");
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
    cbv_desc.SizeInBytes = (this->conf_.size + 0xff & ~0xff);
    device->CreateConstantBufferView(&cbv_desc, this->conf_.descriptor_heap->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
    if (FAILED(this->resource_->SetName(L"Constant Buffer View"))) {
        throw std::runtime_error("ConstantBuffer::createView() : resource->SetName() Failed.");
    }
}