#include "VertexBuffer.h"
#include <stdexcept>

void VertexBuffer::init(ID3D12Device* device)
{
	D3D12_HEAP_PROPERTIES heap_prop = {};
	heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_prop.CreationNodeMask = 1;
	heap_prop.VisibleNodeMask = 1;
	heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = this->conf_.size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	createCommittedResource(
		device,
		heap_prop,
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr
	);

	setName(L"VertexBuffer");

	//頂点バッファビューの設定
	this->vertex_buffer_view_.BufferLocation = getGPUVirtualAddress();
	this->vertex_buffer_view_.SizeInBytes = this->conf_.size;
	this->vertex_buffer_view_.StrideInBytes = this->conf_.stride;
}