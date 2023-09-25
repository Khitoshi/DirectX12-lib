#include "IndexBuffer.h"

void IndexBuffer::init(ID3D12Device* device)
{
	this->size_in_bytes_ = this->conf_.size;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(this->size_in_bytes_);

	auto hr = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(getResourceAddress()));

	//�C���f�b�N�X�o�b�t�@�̃r���[���쐬�B
	this->index_buffer_view_.BufferLocation = getGPUVirtualAddress();

	//�X�g���C�h�͂S�o�C�g�Œ�B
	//this->stride_in_bytes_ = 4;
	this->stride_in_bytes_ = this->conf_.stride;
	//this->index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
	this->index_buffer_view_.Format = DXGI_FORMAT_R16_UINT;
	this->index_buffer_view_.SizeInBytes = this->conf_.stride * this->conf_.count;
}
