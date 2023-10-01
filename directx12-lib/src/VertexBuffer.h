#pragma once
#include "d3dx12.h"
#include "Descriptor.h"

using namespace Microsoft::WRL;

/// <summary>
/// 頂点バッファ生成用&copyするクラス
/// </summary>
class VertexBuffer :public Descriptor
{
	friend class VertexBufferFactory;
public:
	/// <summary>
	/// 頂点バッファ生成時に使用する設定
	/// </summary>
	struct VertexBufferConf {
		UINT size;               //頂点バッファのサイズ
		UINT stride;             //1頂点のサイズ
	};

private:
	VertexBuffer(const VertexBufferConf c) :
		Descriptor(Descriptor::DescriptorType::Vertex),
		conf_(c),
		vertex_buffer_view_()
	{};

public:
	~VertexBuffer() {};

private:
	void init(ID3D12Device* device);

public:
	const D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView() const { return this->vertex_buffer_view_; }
	VertexBufferConf getConf() const { return this->conf_; }
private:
	VertexBufferConf conf_;
	D3D12_VERTEX_BUFFER_VIEW    vertex_buffer_view_;
};
