#pragma once
#include "d3dx12.h"
#include <map>

class DescriptorHeapCache
{
public:
    /// <summary>
    /// �L���b�V���������̐ݒ�
    /// </summary>
    struct DescriptorHeapCacheConf
    {
        ID3D12Resource* resource;   //���\�[�X
        UINT slot;                  //�X���b�g
    };

public:
    DescriptorHeapCache() {}
    ~DescriptorHeapCache() {}

    /// <summary>
    /// �擾or�쐬
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <param name="conf">�������̐ݒ�</param>
    /// <param name="descriptorHeap">�f�B�X�N���v�^�q�[�v</param>
    /// <param name="desc">SRV�̐ݒ�</param>
    void getOrCreate(ID3D12Device* device, DescriptorHeapCacheConf conf, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {
        //���łɍ쐬�ς݂̏ꍇ�͉������Ȃ�
        auto it = this->handleMapCache_.find(conf.slot);
        if (it != this->handleMapCache_.end() && it->second == conf.resource) {
            return;
        }

        //�쐬
        create(device, conf, descriptorHeap, desc);
    }

private:

    /// <summary>
    /// ���\�[�X�쐬
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <param name="conf">�������̐ݒ�</param>
    /// <param name="descriptorHeap">�f�B�X�N���v�^�q�[�v</param>
    /// <param name="desc">SRV�̐ݒ�</param>
    void create(ID3D12Device* device, DescriptorHeapCacheConf conf, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += static_cast<unsigned long long>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * conf.slot;

        device->CreateShaderResourceView(conf.resource, &desc, handle);
        handleMapCache_[conf.slot] = conf.resource;
    }

private:
    std::map<UINT, ID3D12Resource*> handleMapCache_;    //slot���L�[�ɂ������\�[�X�̃}�b�v
};