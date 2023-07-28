#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// インデックスバッファにcopyするクラス
/// </summary>
class IndexBuffer
{
    friend class IndexBufferFactory;
public:
    /// <summary>
    /// インデックスバッファ生成時に使用する設定
    /// </summary>
    struct IndexBufferConf {
        int size;
        int stride;
        int count;
    };

private:
    IndexBuffer(IndexBufferConf c) :
        conf_(c),
        index_buffer_(),
        index_buffer_view_(),
        stride_in_bytes_(0),
        count_(0),
        size_in_bytes_(0)
    {}

public:
    ~IndexBuffer() {}

private:
    //初期化処理
    void init(ID3D12Device* device);

public:
    //リソースにコピー
    void copy(uint16_t* src_indices);
    void copy(uint32_t* src_indices);

public://取得系
    //インデックスバッファビューの取得
    const D3D12_INDEX_BUFFER_VIEW& getIndexBufferView() const { return this->index_buffer_view_; }
    //インデックスバッファのストライドの取得
    int getStrideInBytes() const { return this->stride_in_bytes_; }
    //インデックスバッファの数の取得
    int getCount() const { return this->count_; }
    //インデックスバッファのサイズの取得
    int getSizeInBytes() const { return this->size_in_bytes_; }

private:
    IndexBufferConf conf_;
    ComPtr<ID3D12Resource>  index_buffer_;		//インデックスバッファ。
    D3D12_INDEX_BUFFER_VIEW index_buffer_view_;	//インデックスバッファビュー
    int stride_in_bytes_;						//インデックスバッファのストライド
    int count_;									//インデックスバッファの数
    int size_in_bytes_;							//インデックスバッファのサイズ
};
