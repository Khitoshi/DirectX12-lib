#include "OffScreenRenderTarget.h"
#include <stdexcept>

/// <summary>
/// 初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::init(ID3D12Device* device)
{
	createResource(device);
	createDescriptorHeap(device);
	createView(device);
	createDescriptorHeapSize(device);
	createShaderResourceView(device);
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
	clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM	, color);

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

/// <summary>
/// ディスクリプタヒープ作成
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createDescriptorHeap(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = conf.descriptorHeapDesc;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->descriptorHeap.ReleaseAndGetAddressOf())))){
		throw std::runtime_error("failed to create offscreen render target descriptor heap");
	}
}

/// <summary>
/// レンダーターゲットビューを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createView(ID3D12Device* device)
{
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	device->CreateRenderTargetView(this->resource.Get(), &desc, this->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void OffScreenRenderTarget::createDescriptorHeapSize(ID3D12Device* device)
{
	this->descriptorHeapSize = device->GetDescriptorHandleIncrementSize(conf.descriptorHeapDesc.Type);
}

void OffScreenRenderTarget::createShaderResourceView(ID3D12Device* device)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Format = this->conf.backBufferDesc.Format;
	desc.Texture2D.MipLevels = 1;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	device->CreateShaderResourceView(this->resource.Get(), &desc, this->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
}
