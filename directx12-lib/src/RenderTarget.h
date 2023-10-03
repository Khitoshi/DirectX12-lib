#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
#include "Descriptor.h"

class DescriptorHeap;

using namespace Microsoft::WRL;

class RenderTarget :public Descriptor
{
	friend class RenderTargetFactory;
private:
	RenderTarget(IDXGISwapChain3* swap_chain, UINT buffer, D3D12_CPU_DESCRIPTOR_HANDLE handle) :
		Descriptor(Descriptor::DescriptorType::MainRenderTarget),
		swap_chain_(swap_chain),
		buffer_(buffer),
		handle_(handle),
		resouce_status_()
	{};

	/*
	RenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_RESOURCE_STATES status) :
		Descriptor(Descriptor::DescriptorType::RenderTarget),
		swap_chain_(nullptr),
		buffer_(NULL),
		handle_(handle),
		resouce_status_(status)
	{};
	*/

public:
	~RenderTarget() {};

private:
	void init(ID3D12Device* device)override;

public://Žæ“¾Œn

	CD3DX12_RESOURCE_BARRIER begin();
	CD3DX12_RESOURCE_BARRIER end();

	D3D12_RESOURCE_DESC getDesc();

public:
	IDXGISwapChain3* swap_chain_;
	UINT buffer_;
	const D3D12_CPU_DESCRIPTOR_HANDLE handle_;

	D3D12_RESOURCE_STATES resouce_status_;
};
