#pragma once
#include "d3dx12.h"
#include "Descriptor.h"

using namespace Microsoft::WRL;

class VertexBuffer :public Descriptor
{
	friend class VertexBufferFactory;
public:

	struct VertexBufferConf {
		UINT size;
		UINT stride;
	};

private:
	VertexBuffer(const VertexBufferConf c) :
		Descriptor(Descriptor::DescriptorType::Vertex),
		conf_(c),
		vertex_buffer_view_()
	{};

public:
	~VertexBuffer() {};


	void init(ID3D12Device* device)override;

public://Žæ“¾Œn
	const D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView() const { return this->vertex_buffer_view_; }
	VertexBufferConf getConf() const { return this->conf_; }

private:
	VertexBufferConf conf_;
	D3D12_VERTEX_BUFFER_VIEW    vertex_buffer_view_;
};
