#include "IndexBuffer.h"

void IndexBuffer::init(IndexBufferConf conf)
{
    if (conf.stride == 2) {
        sizeInBytes = conf.size * 2;
    }
    else {
        sizeInBytes = conf.size;
    }

    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);

    auto hr = conf.device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &rDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&indexBuffer));

    //インデックスバッファのビューを作成。
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();

    //ストライドは４バイト固定。
    strideInBytes = 4;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    indexBufferView.SizeInBytes = sizeInBytes;
    count = sizeInBytes / strideInBytes;
}

void IndexBuffer::copy(uint16_t* srcIndices)
{
    uint32_t* pData = nullptr;
    if (FAILED(this->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < count; i++) {
            pData[i] = srcIndices[i];
        }
    }
    this->indexBuffer->Unmap(0, nullptr);

}

void IndexBuffer::copy(uint32_t* srcIndices)
{
    uint32_t* pData = nullptr;

    if (FAILED(this->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < count; i++) {
            pData[i] = srcIndices[i];
        }
    }
    this->indexBuffer->Unmap(0, nullptr);

}