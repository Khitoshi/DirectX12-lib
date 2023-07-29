#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// 頂点バッファ生成用&copyするクラス
/// </summary>
class VertexBuffer
{
    friend class VertexBufferFactory;
public:
    /// <summary>
    /// 頂点バッファ生成時に使用する設定
    /// </summary>
    struct VertexBufferConf {
        int size;               //頂点バッファのサイズ
        int stride;             //1頂点のサイズ
    };

private:
    VertexBuffer(const VertexBufferConf c) :
        conf_(c),
        vertex_buffer_(),
        vertex_buffer_view_()
    {};

public:
    ~VertexBuffer() {};

private:
    //初期化処理
    void init(ID3D12Device* device);

public:
    //リソースにコピー
    void copy(void* srcVertices);

public:
    //頂点バッファビューの取得
    const D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView() const { return this->vertex_buffer_view_; }

private:
    VertexBufferConf conf_;
    ComPtr<ID3D12Resource>      vertex_buffer_;	        //頂点バッファ。
    D3D12_VERTEX_BUFFER_VIEW    vertex_buffer_view_;	//頂点バッファビュー。
};
