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


