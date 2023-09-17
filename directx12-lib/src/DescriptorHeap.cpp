#include "DescriptorHeap.h"
#include <stdexcept>
#include "DescriptorHeapManager.h"
void DescriptorHeap::init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors)
{
	this->type_ = type;
	this->num_descriptors_ = num_descriptors;

	//�f�B�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = this->num_descriptors_;
	desc.Type = this->type_;


	if (desc.Type != D3D12_DESCRIPTOR_HEAP_TYPE_RTV && desc.Type != D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}
	else {
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	}

	//�f�B�X�N���v�^�q�[�v�̍쐬
	if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->descriptor_heap_)))) {
		throw std::runtime_error("failed to create descriptor heap");
	}

	DescriptorHeapManager::regist(this->type_, this);
}

void DescriptorHeap::reinit(ID3D12Device* device)
{
	//�f�B�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = this->num_descriptors_;
	desc.Type = this->type_;
	//desc.Flags = ? : ;
	if (desc.Type != D3D12_DESCRIPTOR_HEAP_TYPE_RTV && desc.Type != D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}
	else {
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	}

	//�f�B�X�N���v�^�q�[�v�̍쐬
	if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->descriptor_heap_)))) {
		throw std::runtime_error("failed to create descriptor heap");
	}
}

