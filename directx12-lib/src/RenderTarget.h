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

    void resourceReset(const UINT i) { resource_[i].Reset(); }

    void createResource(ID3D12Device* device);
private:
    void init(ID3D12Device* device);
    void createDescriptorHeap(ID3D12Device* device);
    void createDescriptorHeapSize(ID3D12Device* device);

public://�擾�n
    ID3D12DescriptorHeap* getDescriptorHeap() const;
    int getDescriptorHeapSize() const { return this->descriptor_heap_size_; }
    ID3D12Resource* getResource(int i) const { return resource_[i].Get(); }

private:
    RenderTargetConf conf_;
    std::shared_ptr<DescriptorHeap> descriptor_heap_;

    int descriptor_heap_size_;
    std::vector<ComPtr<ID3D12Resource>> resource_;
};
