#include "ConstantBuffer.h"
#include <stdexcept>
void ConstantBuffer::init(ID3D12Device* device)
{
	this->createResource(device);
	this->createDescriptorHeap(device);
	this->createView(device);
}

void ConstantBuffer::createResource(ID3D12Device* device)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto buf = CD3DX12_RESOURCE_DESC::Buffer((sizeof(DirectX::XMMATRIX) + 0xff) & ~0xff);
	
	device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE, 
		&buf,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&this->resource));
	if (FAILED(this->resource->SetName(L"Constant Buffer Upload Resource Heap"))){
		throw std::runtime_error("ConstantBuffer::createResource() : resource->SetName() Failed.");
	}
}

void ConstantBuffer::createDescriptorHeap(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	
	if (FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&this->descriptorHeap)))){
		throw std::runtime_error("ConstantBuffer::createDescriptorHeap() : device->CreateDescriptorHeap() Failed.");
	}

	if (FAILED(this->descriptorHeap->SetName(L"Constant Buffer Descriptor Heap"))){
		throw std::runtime_error("ConstantBuffer::createDescriptorHeap() : descriptorHeap->SetName() Failed.");
	}
}

void ConstantBuffer::createView(ID3D12Device* device)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = this->resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(DirectX::XMMATRIX) + 0xff) & ~0xff;
	device->CreateConstantBufferView(&cbvDesc, this->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
	if (FAILED(this->resource->SetName(L"Constant Buffer View"))) {
		throw std::runtime_error("ConstantBuffer::createView() : resource->SetName() Failed.");
	}
}

