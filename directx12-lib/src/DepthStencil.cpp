#include "DepthStencil.h"
#include <stdexcept>

void DepthStencil::init(const DepthStencilConf depthStencilConf)
{
    this->descriptorHeap = createDescriptorHeap(depthStencilConf);
    createResource(depthStencilConf);
}

ComPtr<ID3D12DescriptorHeap> DepthStencil::createDescriptorHeap(const DepthStencilConf depthStencilConf)
{
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};

    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.NodeMask = 0;
    desc.NumDescriptors = depthStencilConf.frameBufferCount;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(depthStencilConf.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)))) {
        throw std::runtime_error("failed to create depth stencil descriptor heap");
    }

    return descriptorHeap;
}

/*
int DepthStencil::createDescriptorHeapSize(const DepthStencilConf depthStencilConf)
{
    return -1;
}
*/

void DepthStencil::createResource(const DepthStencilConf depthStencilConf)
{
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
    if (FAILED(depthStencilConf.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resouceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsvClearValue, IID_PPV_ARGS(&this->resource))
    )) {
        throw std::runtime_error("failed to create depth stencil resource");
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(this->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
    depthStencilConf.device->CreateDepthStencilView(this->resource.Get(), &dsvDesc, dsvHandle);
}
