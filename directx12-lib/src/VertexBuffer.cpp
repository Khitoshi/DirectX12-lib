#include "VertexBuffer.h"
#include <stdexcept>

/// <summary>
/// 頂点バッファの初期化
/// </summary>
/// <param name="conf">頂点バッファ生成用の設定</param>
void VertexBuffer::init(ID3D12Device* device)
{
    //ヒープの設定
    D3D12_HEAP_PROPERTIES heap_prop = {};
    heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_prop.CreationNodeMask = 1;
    heap_prop.VisibleNodeMask = 1;
    heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = this->conf_.size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    if (FAILED(
        device->CreateCommittedResource(
            &heap_prop,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&this->vertex_buffer_)
        )
    )) {
        throw std::runtime_error("failed to create vertex buffer");
    }
    if (FAILED(this->vertex_buffer_->SetName(L"VertexBuffer"))) {
        throw std::runtime_error("failed to set name to vertex buffer");
    }

    //頂点バッファビューの設定
    this->vertex_buffer_view_.BufferLocation = this->vertex_buffer_->GetGPUVirtualAddress();
    this->vertex_buffer_view_.SizeInBytes = this->conf_.size;
    this->vertex_buffer_view_.StrideInBytes = this->conf_.stride;
}

/// <summary>
/// リソースに頂点データをコピーする。
/// </summary>
/// <param name="srcVertices">頂点データ配列</param>
void VertexBuffer::copy(void* src_vertices)
{
    uint8_t* pData = nullptr;
    //マッピング開始
    if (FAILED(this->vertex_buffer_->Map(0, nullptr, (void**)&pData))) {
        throw std::runtime_error("failed to map vertex buffer");
    }
    //頂点データをマッピング先に登録
    memcpy(pData, src_vertices, this->vertex_buffer_view_.SizeInBytes);
    //マッピング登録
    this->vertex_buffer_->Unmap(0, nullptr);
}