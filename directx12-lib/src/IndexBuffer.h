#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

struct IndexBufferConf {
	ID3D12Device* device;
	int size;
	int stride;
};

class IndexBuffer
{
public:
	IndexBuffer():
		indexBuffer(),
		indexBufferView(),
		strideInBytes(0),
		count(0),
		sizeInBytes(0)
	
	{}
	~IndexBuffer() {}

	void init(IndexBufferConf conf);
	void copy(uint16_t* srcIndices);
	void copy(uint32_t* srcIndices);

public:
	//インデックスバッファビューの取得
	D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const { return indexBufferView; }

	//インデックスバッファのストライドの取得
	int getStrideInBytes() const { return strideInBytes; }

	//インデックスバッファの数の取得
	int getCount() const { return count; }

	//インデックスバッファのサイズの取得
	int getSizeInBytes() const { return sizeInBytes; }
private:
	ID3D12Resource* indexBuffer = nullptr;		//インデックスバッファ。
	D3D12_INDEX_BUFFER_VIEW indexBufferView;	//インデックスバッファビュー。
	int strideInBytes;							//インデックスバッファのストライド。
	int count;									//インデックスバッファの数。
	int sizeInBytes;							//インデックスバッファのサイズ。
};
