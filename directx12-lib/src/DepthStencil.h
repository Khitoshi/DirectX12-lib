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
    DepthStencil() {};
    ~DepthStencil() {};

    void init(const DepthStencilConf depthStencilConf);

private:
    //�f�B�X�N���v�^�q�[�v�̐���
    ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(const DepthStencilConf depthStencilConf);

    //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
    int createDescriptorHeapSize(const DepthStencilConf depthStencilConf);

    //���\�[�X�̐���
    ComPtr<ID3D12Resource> createResource(const DepthStencilConf depthStencilConf);


private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //�f�B�X�N���v�^�q�[�v
    int descriptorHeapSize;                         //�f�B�X�N���v�^�q�[�v�̃T�C�Y
    ComPtr<ID3D12Resource> resource;   //���\�[�X
};
