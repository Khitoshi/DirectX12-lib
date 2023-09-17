#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include <stdexcept>

/*
void ConstantBuffer::copy(void* src_constants)
{
	this->descriptor_->map(src_constants, this->conf_.size);
}
*/

void ConstantBuffer::init(ID3D12Device* device)
{
	this->createResource(device);
	this->createView(device);

	setName(L"ConstantBuffer");
}

void ConstantBuffer::createResource(ID3D12Device* device)
{
	auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto buf = CD3DX12_RESOURCE_DESC::Buffer(this->conf_.size + 0xff & ~0xff);

	createCommittedResource(device, heap_prop, D3D12_HEAP_FLAG_NONE, buf, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
}

void ConstantBuffer::createView(ID3D12Device* device)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
	cbv_desc.BufferLocation = getGPUVirtualAddress();
	cbv_desc.SizeInBytes = (this->conf_.size + 0xff & ~0xff);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = this->conf_.descriptor_heap->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += static_cast<unsigned long long>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * this->conf_.slot;

	//this->descriptor_->createCBV(device, cbv_desc, handle);
	device->CreateConstantBufferView(&cbv_desc, handle);
}