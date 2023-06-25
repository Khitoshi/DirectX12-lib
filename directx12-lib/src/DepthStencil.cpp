#include "DepthStencil.h"
#include <stdexcept>

/// <summary>
/// �[�x�X�e���V���o�b�t�@�̏���������
/// </summary>
/// <param name="depthStencilConf"></param>
void DepthStencil::init(const DepthStencilConf depthStencilConf)
{
    this->descriptorHeap = createDescriptorHeap(depthStencilConf);
    createResource(depthStencilConf);
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̐���
/// </summary>
/// <param name="depthStencilConf"></param>
/// <returns></returns>
ComPtr<ID3D12DescriptorHeap> DepthStencil::createDescriptorHeap(const DepthStencilConf depthStencilConf)
{
    //�f�B�X�N���v�^�q�[�v�̐ݒ�
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.NodeMask = 0;
    desc.NumDescriptors = depthStencilConf.frameBufferCount;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    //�f�B�X�N���v�^�q�[�v�̐���
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    if (FAILED(depthStencilConf.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)))) {
        throw std::runtime_error("failed to create depth stencil descriptor heap");
    }
    return descriptorHeap;
}

/// <summary>
/// ���\�[�X����
/// </summary>
/// <param name="depthStencilConf"></param>
void DepthStencil::createResource(const DepthStencilConf depthStencilConf)
{
    //�[�x�X�e���V���r���[�p���\�[�X�̐ݒ�
    D3D12_CLEAR_VALUE dsvClearValue = {};
    dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    dsvClearValue.DepthStencil.Depth = 1.0f;
    dsvClearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_DESC resouceDesc = {};
    resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resouceDesc.Width = depthStencilConf.width;
    resouceDesc.Height = depthStencilConf.height;
    resouceDesc.DepthOrArraySize = 1;
    resouceDesc.MipLevels = 1;
    resouceDesc.Format = DXGI_FORMAT_D32_FLOAT;
    resouceDesc.SampleDesc.Count = 1;
    resouceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    //�[�x�X�e���V���r���[�p���\�[�X�̐���
    if (FAILED(depthStencilConf.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resouceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsvClearValue, IID_PPV_ARGS(&this->resource))
    )) {
        throw std::runtime_error("failed to create depth stencil resource");
    }

    //�[�x�X�e���V���r���[�̐ݒ�
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    //�[�x�X�e���V���r���[�̐���
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(this->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
    depthStencilConf.device->CreateDepthStencilView(this->resource.Get(), &dsvDesc, dsvHandle);
}
