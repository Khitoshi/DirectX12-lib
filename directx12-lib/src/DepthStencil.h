#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>

class DescriptorHeap;

using namespace Microsoft::WRL;

/// <summary>
/// �[�x�X�e���V���o�b�t�@�����p�N���X
/// </summary>
class DepthStencil
{
    friend class DepthStencilFactory;
public:
    /// <summary>
    /// �[�x�X�e���V���o�b�t�@�������Ɏg�p����ݒ�
    /// </summary>
    struct DepthStencilConf
    {
        UINT frame_buffer_count;    //�t���[���o�b�t�@��
        UINT width;                 //��
        UINT height;                //����

        bool operator==(const DepthStencilConf& conf) const
        {
            return
                frame_buffer_count == conf.frame_buffer_count &&
                width == conf.width &&
                height == conf.height;
        }
    };

private:
    DepthStencil(const DepthStencilConf c) :conf_(c), descriptor_heap_(), resource_() {};

public:
    ~DepthStencil() {};

private:
    //����������
    void init(ID3D12Device* device);
    //�f�B�X�N���v�^�q�[�v�̐���
    void createDescriptorHeap(ID3D12Device* device);
    //���\�[�X�̐���
    void createResource(ID3D12Device* device);
    //�[�x�X�e���V���r���[����
    void createDSV(ID3D12Device* device);

public:
    //�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* getDescriptorHeap() const;

    //���\�[�X�̎擾
    ID3D12Resource* getResource() const { return resource_.Get(); }

private:
    DepthStencilConf conf_;                             //�[�x�X�e���V���o�b�t�@�������Ɏg�p����ݒ�
    //ComPtr<ID3D12DescriptorHeap>    descriptor_heap_;   //�f�B�X�N���v�^�q�[�v
    std::shared_ptr<DescriptorHeap>    descriptor_heap_;   //�f�B�X�N���v�^�q�[�v
    ComPtr<ID3D12Resource>          resource_;          //���\�[�X
};
