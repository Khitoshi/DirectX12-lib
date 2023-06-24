#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
using namespace Microsoft::WRL;

struct DepthStencilConf
{
    ID3D12Device* device;
    UINT frameBufferCount;
    UINT width;
    UINT height;
};


class DepthStencil
{
public:
    DepthStencil() {};
    ~DepthStencil() {};

    void init(const DepthStencilConf depthStencilConf);

private:
    //�f�B�X�N���v�^�q�[�v�̐���
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const DepthStencilConf depthStencilConf);

    //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
    //int createDescriptorHeapSize(const DepthStencilConf depthStencilConf);

    //���\�[�X�̐���
    void createResource(const DepthStencilConf depthStencilConf);

public:
    ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptorHeap.Get(); } //�f�B�X�N���v�^�q�[�v�̎擾
    //int getDescriptorHeapSize() const { return descriptorHeapSize; } //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
    ID3D12Resource* getResource() const { return resource.Get(); } //���\�[�X�̎擾
private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //�f�B�X�N���v�^�q�[�v
    //int descriptorHeapSize;                         //�f�B�X�N���v�^�q�[�v�̃T�C�Y
    ComPtr<ID3D12Resource> resource;   //���\�[�X
};
