#pragma once

#include "Descriptor.h"

class DescriptorHeap;

class SRV : public Descriptor
{
	friend class SRVFactory;
private:
	SRV(const D3D12_HEAP_PROPERTIES* heap_prop, const D3D12_RESOURCE_DESC* desc) :
		Descriptor(Descriptor::DescriptorType::ShaderResource),
		heap_prop_(heap_prop),
		desc_(desc)
	{};
public:
	~SRV() {};

	void writeToSubresource(
		const UINT& DstSubresource,
		const void* src,
		const UINT& SrcRowPitch,
		const UINT& SrcDepthPitch);

	void createSRV(ID3D12Device* device, const DXGI_FORMAT& format, DescriptorHeap* descriptor_heap, const UINT slot);

	void init(ID3D12Device* device)override;

private:
	const D3D12_HEAP_PROPERTIES* heap_prop_;
	const D3D12_RESOURCE_DESC* desc_;
};
