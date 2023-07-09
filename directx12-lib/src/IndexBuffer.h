#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// インデックスバッファ生成時に使用する設定
/// </summary>
struct IndexBufferConf {
    ID3D12Device* device;
    int size;
    int stride;
    int count;
};

/// <summary>
/// インデックスバッファ生成用&copyするクラス
/// </summary>
class IndexBuffer
{
public:
    IndexBuffer() :
        indexBuffer(),
        indexBufferView(),
        strideInBytes(0),
        count(0),
        sizeInBytes(0)

    {}
    ~IndexBuffer() {}

    //初期化処理
    void init(IndexBufferConf conf);

    //リソースにコピー
    void copy(uint16_t* srcIndices);
    void copy(uint32_t* srcIndices);

public:
    //インデックスバッファビューの取得
    const D3D12_INDEX_BUFFER_VIEW& getIndexBufferView() const { return indexBufferView; }

    //インデックスバッファのストライドの取得
    int getStrideInBytes() const { return strideInBytes; }

    //インデックスバッファの数の取得
    int getCount() const { return count; }

    //インデックスバッファのサイズの取得
    int getSizeInBytes() const { return sizeInBytes; }
private:
    ComPtr<ID3D12Resource>  indexBuffer;		//インデックスバッファ。
    D3D12_INDEX_BUFFER_VIEW indexBufferView;	//インデックスバッファビュー。
    int strideInBytes;							//インデックスバッファのストライド。
    int count;									//インデックスバッファの数。
    int sizeInBytes;							//インデックスバッファのサイズ。
};
