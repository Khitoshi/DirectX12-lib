#include "Descriptor.h"
#include <stdexcept>

void Descriptor::createCommittedResource(
	ID3D12Device* device,
	const D3D12_HEAP_PROPERTIES& heap,
	const D3D12_HEAP_FLAGS& flag,
	const D3D12_RESOURCE_DESC& desc,
	const D3D12_RESOURCE_STATES& status,
	const D3D12_CLEAR_VALUE* clear_value) {
	if (FAILED(device->CreateCommittedResource(
		&heap,
		flag,
		&desc,
		status,
		clear_value,
		IID_PPV_ARGS(&this->resource_)))) {
		throw std::runtime_error("FAILED Descriptor::create CreateCommittedResource");
	}
}

void Descriptor::setName(const LPCWSTR& name)
{
	if (FAILED(this->resource_->SetName(name))) {
		throw std::runtime_error("FAILED Descriptor::setName SetName");
	}
}


void Descriptor::map(void* src, const UINT size)
{
	void* pData = nullptr;
	if (FAILED(this->resource_->Map(0, nullptr, &pData))) {
		throw std::runtime_error("FAILED Descriptor::map Map");
	}
	memcpy(pData, src, size);
	this->resource_->Unmap(0, nullptr);
}

void Descriptor::createCBV(
	ID3D12Device* device,
	const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc,
	const D3D12_CPU_DESCRIPTOR_HANDLE& handle) {
	device->CreateConstantBufferView(&desc, handle);
}

void Descriptor::createSRV(
	ID3D12Device* device,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& srv_desc,
	const D3D12_CPU_DESCRIPTOR_HANDLE& handle) {
	device->CreateShaderResourceView(this->resource_.Get(), &srv_desc, handle);
}

void Descriptor::writeToSubresource(const UINT& DstSubresource, const void* src, const UINT& SrcRowPitch, const UINT& SrcDepthPitch)
{
	if (FAILED(this->resource_->WriteToSubresource(DstSubresource, nullptr, src, SrcRowPitch, SrcDepthPitch))) {
		throw std::runtime_error("FAILED Descriptor::WriteToSubresource WriteToSubresource");
	}
}

void Descriptor::createRTV(ID3D12Device* device, const D3D12_RENDER_TARGET_VIEW_DESC* desc, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{

}