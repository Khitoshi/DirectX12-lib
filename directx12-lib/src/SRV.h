#pragma once

#include "Descriptor.h"

class DescriptorHeap;

class SRV : public Descriptor
{
	friend class SRVFactory;
private:
	SRV() :
		Descriptor(Descriptor::DescriptorType::ShaderResource) {};
public:
	~SRV() {};

	void writeToSubresource(
		const UINT& DstSubresource,
		const void* src,
		const UINT& SrcRowPitch,
		const UINT& SrcDepthPitch);

	void createSRV(ID3D12Device* device, const DXGI_FORMAT& format, DescriptorHeap* descriptor_heap, const UINT slot);
private:
	void init(ID3D12Device* device, const D3D12_HEAP_PROPERTIES& heap_prop, const D3D12_RESOURCE_DESC& desc);

};
