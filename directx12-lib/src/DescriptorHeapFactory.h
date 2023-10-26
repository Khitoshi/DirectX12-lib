#pragma once

#include "DescriptorHeap.h"
#include <memory>

/// <summary>
/// �f�B�X�N���v�^�q�[�v�t�@�N�g���N���X
/// </summary>
class DescriptorHeapFactory
{
public:

	static std::shared_ptr<DescriptorHeap> create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors)
	{
		std::shared_ptr<DescriptorHeap> heap(new DescriptorHeap());
		heap->init(device, type, num_descriptors);

		return heap;
	}
};
