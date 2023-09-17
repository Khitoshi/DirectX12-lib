#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;



class DescriptorHeap
{
	friend class DescriptorHeapFactory;

private:

	DescriptorHeap() :
		descriptor_heap_(),
		type_(),
		num_descriptors_(0)
	{};
public:
	~DescriptorHeap() {};
private:
	void init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors);

public:
	void clear() { this->descriptor_heap_->Release(); }
	void reinit(ID3D12Device* device);

public:
	ID3D12DescriptorHeap* getDescriptorHeap() { return descriptor_heap_.Get(); }

private:

	ComPtr<ID3D12DescriptorHeap> descriptor_heap_;

	D3D12_DESCRIPTOR_HEAP_TYPE type_;
	UINT num_descriptors_;
};
