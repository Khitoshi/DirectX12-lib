#pragma once
#include "d3dx12.h"
#include "Descriptor.h"

class IndexBuffer : public Descriptor
{
	friend class IndexBufferFactory;
public:
	struct IndexBufferConf {
		int size;
		int stride;
		int count;
	};

private:
	IndexBuffer(IndexBufferConf c) :
		conf_(c),
		index_buffer_view_(),
		stride_in_bytes_(0),
		size_in_bytes_(0)
	{}

public:
	~IndexBuffer() {}

private:
	void init(ID3D12Device* device);

public://Žæ“¾Œn
	const D3D12_INDEX_BUFFER_VIEW& getIndexBufferView() const { return this->index_buffer_view_; }
	int getStrideInBytes() const { return this->stride_in_bytes_; }
	int getSizeInBytes() const { return this->size_in_bytes_; }

private:
	IndexBufferConf conf_;
	D3D12_INDEX_BUFFER_VIEW index_buffer_view_;
	int stride_in_bytes_;
	int size_in_bytes_;
};
