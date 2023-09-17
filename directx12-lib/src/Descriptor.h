#pragma once
#include "d3dx12.h"
#include <string>
#include <chrono>
#include <dxgi1_4.h>

using namespace Microsoft::WRL;

class Descriptor
{
protected:
	Descriptor() :
		resource_()
	{}

public:
	~Descriptor() {}

protected:

	void createCommittedResource(
		ID3D12Device* device,
		const D3D12_HEAP_PROPERTIES& heap,
		const D3D12_HEAP_FLAGS& flag,
		const D3D12_RESOURCE_DESC& desc,
		const D3D12_RESOURCE_STATES& status,
		const D3D12_CLEAR_VALUE* clear_value
	);

	void setName(const LPCWSTR& name);
public:
	void map(void* src, const UINT size);

private:
	void createCBV(
		ID3D12Device* device,
		const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc,
		const D3D12_CPU_DESCRIPTOR_HANDLE& handle
	);

	void createSRV(
		ID3D12Device* device,
		const D3D12_SHADER_RESOURCE_VIEW_DESC& srv_desc,
		const D3D12_CPU_DESCRIPTOR_HANDLE& handle);

	void writeToSubresource(
		const UINT& DstSubresource,
		const void* src,
		const UINT& SrcRowPitch,
		const UINT& SrcDepthPitch);

	void createRTV(
		ID3D12Device* device,
		const D3D12_RENDER_TARGET_VIEW_DESC* desc,
		const D3D12_CPU_DESCRIPTOR_HANDLE& handle);

public:
	D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress()
	{
		return this->resource_->GetGPUVirtualAddress();
	}

	ID3D12Resource* getResource()const
	{
		return this->resource_.Get();
	}

protected:
	//TODO:private‚É‚·‚é
	ComPtr<ID3D12Resource> resource_;

};
