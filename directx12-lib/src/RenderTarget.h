#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
using namespace Microsoft::WRL;

/// <summary>
/// �����_�[�^�[�Q�b�g�������Ɏg�p����ݒ�
/// </summary>
struct RenderTargetConf {
    int width;
    int height;
    UINT frameBufferCount;
    ID3D12Device* device;
    IDXGISwapChain3* swapChain;
};

class RenderTarget
{
public:
    RenderTarget() :
        descriptorHeap(),
        descriptorHeapSize(0),
        resource()
    {};
    ~RenderTarget() {};

    //����������
    void init(RenderTargetConf renderTargetConf);
private:
    //�f�B�X�N���v�^�q�[�v�̐���
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const RenderTargetConf renderTargetConf);

    //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
    int createDescriptorHeapSize(const RenderTargetConf renderTargetConf);

    //���\�[�X�̐���
    std::vector<ComPtr<ID3D12Resource>> createResource(const RenderTargetConf renderTargetConf);

public:
    ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptorHeap.Get(); } //�f�B�X�N���v�^�q�[�v�̎擾
    int getDescriptorHeapSize() const { return descriptorHeapSize; } //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
    ID3D12Resource* getResource(int index) const { return resource[index].Get(); } //���\�[�X�̎擾

private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //�f�B�X�N���v�^�q�[�v
    int descriptorHeapSize;                         //�f�B�X�N���v�^�q�[�v�̃T�C�Y
    std::vector<ComPtr<ID3D12Resource>> resource;   //���\�[�X
};
