#include "OffScreenRenderTarget.h"
#include <stdexcept>

/// <summary>
/// 初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::init(ID3D12Device* device)
{
    createResource(device);
    //createDescriptorHeap(device);
    //createView(device);
    createDescriptorHeapSize(device);

    createShaderResourceView(device);
    createRenderTargetView(device);
}


/// <summary>
/// リソースを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createResource(ID3D12Device* device)
{
    D3D12_RESOURCE_DESC desc = {};
    desc = this->conf.backBufferDesc;
    D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, color);

    if (FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(this->resource.ReleaseAndGetAddressOf())
    ))) {
        throw std::runtime_error("failed to create offscreen render target");
    }
    return;
}

void OffScreenRenderTarget::createDescriptorHeapSize(ID3D12Device* device)
{
    this->descriptorHeapSize = device->GetDescriptorHandleIncrementSize(conf.descriptorHeapDesc.Type);
}

void OffScreenRenderTarget::createShaderResourceView(ID3D12Device* device)
{
    //SRVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf.descriptorHeapDesc;
    desc.NumDescriptors = 2;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->SRVHeap.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }

    //シェーダーリソースビューを作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    device->CreateShaderResourceView(this->resource.Get(), &srvDesc, this->SRVHeap->GetCPUDescriptorHandleForHeapStart());


}

void OffScreenRenderTarget::createRenderTargetView(ID3D12Device* device)
{
    //RTVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf.descriptorHeapDesc;
    desc.NumDescriptors = 1;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->RTVHeap.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }

    //レンダーターゲットビューを作成
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateRenderTargetView(this->resource.Get(), &rtvDesc, this->RTVHeap->GetCPUDescriptorHandleForHeapStart());
    return;
}
