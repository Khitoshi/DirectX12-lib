#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// 頂点バッファ生成時に使用する設定
/// </summary>
struct VertexBufferConf {
    ID3D12Device* device;
    int size;
    int stride;
};

/// <summary>
/// 頂点バッファ生成用&copyするクラス
/// </summary>
class VertexBuffer
{
public:
    VertexBuffer() :vertexBuffer(), vertexBufferView() {};
    ~VertexBuffer() {};

    //初期化処理
    void init(VertexBufferConf conf);
    //リソースにコピー
    void copy(void* srcVertices);

public:
    //頂点バッファビューの取得
    D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const { return vertexBufferView; }

private:
    ComPtr<ID3D12Resource>      vertexBuffer;		        //頂点バッファ。
    D3D12_VERTEX_BUFFER_VIEW    vertexBufferView;	        //頂点バッファビュー。
};
