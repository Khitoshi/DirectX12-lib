#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>

class DescriptorHeap;

using namespace Microsoft::WRL;

/// <summary>
/// �����_�[�^�[�Q�b�g�����p�N���X
/// </summary>
class RenderTarget
{
    friend class RenderTargetFactory;
public:
    /// <summary>
    /// �����_�[�^�[�Q�b�g�������Ɏg�p����ݒ�
    /// </summary>
    struct RenderTargetConf {
        UINT frame_buffer_count;        //�t���[���o�b�t�@�̐�
        IDXGISwapChain3* swap_chain;    //�X���b�v�`�F�C��
    };

private:
    RenderTarget(const RenderTargetConf c) :
        conf_(c),
        descriptor_heap_(),
        descriptor_heap_size_(0),
        resource_()
    {};

public:
    ~RenderTarget() {};

private:
    //����������
    void init(ID3D12Device* device);
    //�f�B�X�N���v�^�q�[�v�̐���
    void createDescriptorHeap(ID3D12Device* device);
    //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
    void createDescriptorHeapSize(ID3D12Device* device);
    //���\�[�X�̐���
    void createResource(ID3D12Device* device);

public:
    //�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* getDescriptorHeap() const;
    //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
    int getDescriptorHeapSize() const { return this->descriptor_heap_size_; }
    //���\�[�X�̎擾
    ID3D12Resource* getResource(int index) const { return resource_[index].Get(); }

private:
    RenderTargetConf conf_;                         //�ݒ�

    //ComPtr<ID3D12DescriptorHeap> descriptor_heap_;  //�f�B�X�N���v�^�q�[�v
    std::shared_ptr<DescriptorHeap> descriptor_heap_;  //�f�B�X�N���v�^�q�[�v

    int descriptor_heap_size_;                      //�f�B�X�N���v�^�q�[�v�̃T�C�Y
    std::vector<ComPtr<ID3D12Resource>> resource_;  //���\�[�X
};
