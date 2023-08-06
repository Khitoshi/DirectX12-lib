#pragma once

#include "DescriptorHeap.h"
#include <memory>

/// <summary>
/// �f�B�X�N���v�^�q�[�v�����N���X
/// </summary>
class DescriptorHeapFactory
{
public:
    /// <summary>
    /// ����
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <param name="type">�f�B�X�N���v�^�q�[�v�^�C�v</param>
    /// <param name="num_descriptors">�f�B�X�N���v�^�̐�</param>
    /// <returns>
    /// ����&���������ꂽ�f�B�X�N���v�^�q�[�v
    /// </returns>
    static std::shared_ptr<DescriptorHeap> create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors)
    {
        std::shared_ptr<DescriptorHeap> heap(new DescriptorHeap());
        heap->init(device, type, num_descriptors);

        return heap;
    }
};
