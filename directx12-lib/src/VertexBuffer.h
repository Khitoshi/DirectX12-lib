#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

struct VertexBufferConf {
	ID3D12Device* device;
	int size;
	int stride;
};

class VertexBuffer
{
public:
	VertexBuffer():vertexBuffer(), vertexBufferView(){};
	~VertexBuffer() {};

	void init(VertexBufferConf conf);
	void copy(void* srcVertices);

public:
	//頂点バッファビューの取得
	D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const { return vertexBufferView; }
		
private:
	ID3D12Resource* vertexBuffer = nullptr;		//頂点バッファ。
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;	//頂点バッファビュー。
};
