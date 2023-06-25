#include "IndexBuffer.h"

/// <summary>
/// インデクスバッファの初期化
/// </summary>
/// <param name="conf">インデックスバッファ生成用設定</param>
void IndexBuffer::init(IndexBufferConf conf)
{
    this->sizeInBytes = conf.size;
    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(this->sizeInBytes);

    auto hr = conf.device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &rDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&this->indexBuffer));

    //インデックスバッファのビューを作成。
    this->indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();

    //ストライドは４バイト固定。
    this->strideInBytes = 4;
    this->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    this->indexBufferView.SizeInBytes = sizeof(uint32_t) * conf.count;;
    this->count = conf.count;
}


/// <summary>
/// リソースにインデックスデータをコピーする。
/// </summary>
/// <param name="srcIndices">インデックスデータの配列</param>
void IndexBuffer::copy(uint16_t* srcIndices)
{
    uint32_t* pData = nullptr;
    if (SUCCEEDED(this->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < count; i++) {
            pData[i] = static_cast<uint32_t>(srcIndices[i]);
        }
    }
    this->indexBuffer->Unmap(0, nullptr);

}

/// <summary>
/// リソースにインデックスデータをコピーする。
/// </summary>
/// <param name="srcIndices">インデックスデータの配列</param>
void IndexBuffer::copy(uint32_t* srcIndices)
{
    uint32_t* pData = nullptr;
    if (SUCCEEDED(this->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < count; i++) {
            pData[i] = srcIndices[i];
        }
    }
    this->indexBuffer->Unmap(0, nullptr);
}
