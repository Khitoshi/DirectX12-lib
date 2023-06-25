#include "VertexBuffer.h"
#include <stdexcept>

/// <summary>
/// 頂点バッファの初期化
/// </summary>
/// <param name="conf">頂点バッファ生成用の設定</param>
void VertexBuffer::init(VertexBufferConf conf)
{
    //ヒープの設定
    D3D12_HEAP_PROPERTIES heapProp = {};
    heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProp.CreationNodeMask = 1;
    heapProp.VisibleNodeMask = 1;
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = conf.size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    if (FAILED(
        conf.device->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBuffer)
        )
    )) {
        throw std::runtime_error("failed to create vertex buffer");
    }
    vertexBuffer->SetName(L"VertexBuffer");

    //頂点バッファビューの設定
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = conf.size;
    vertexBufferView.StrideInBytes = conf.stride;
}

/// <summary>
/// リソースに頂点データをコピーする。
/// </summary>
/// <param name="srcVertices">頂点データ配列</param>
void VertexBuffer::copy(void* srcVertices)
{
    uint8_t* pData = nullptr;
    //マッピング開始
    if (FAILED(vertexBuffer->Map(0, nullptr, (void**)&pData))) {
        throw std::runtime_error("failed to map vertex buffer");
    }
    //頂点データをマッピング先に登録
    memcpy(pData, srcVertices, vertexBufferView.SizeInBytes);
    //マッピング登録
    vertexBuffer->Unmap(0, nullptr);
}
