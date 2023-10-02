#include "DepthStencil.h"
#include "DescriptorHeapFactory.h"
#include <stdexcept>

/// <summary>
/// 深度ステンシルバッファの初期化処理
/// </summary>
/// <param name="device">GPUデバイス</param>
void DepthStencil::init(ID3D12Device* device)
{
	createDescriptorHeap(device);
	createResource(device);
	createDSV(device);
}

/// <summary>
/// ディスクリプタヒープの生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void DepthStencil::createDescriptorHeap(ID3D12Device* device)
{
	//ディスクリプタヒープの設定
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, this->conf_.frame_buffer_count);
}

/// <summary>
/// リソース生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void DepthStencil::createResource(ID3D12Device* device)
{
	//深度ステンシルビュー用リソースの設定
	D3D12_CLEAR_VALUE dsv_clear_value = {};
	dsv_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	dsv_clear_value.DepthStencil.Depth = 1.0f;
	dsv_clear_value.DepthStencil.Stencil = 0;

	D3D12_RESOURCE_DESC resouce_desc = {};
	resouce_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resouce_desc.Width = this->conf_.width;
	resouce_desc.Height = this->conf_.height;
	resouce_desc.DepthOrArraySize = 1;
	resouce_desc.MipLevels = 1;
	resouce_desc.Format = DXGI_FORMAT_D32_FLOAT;
	resouce_desc.SampleDesc.Count = 1;
	resouce_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	CD3DX12_HEAP_PROPERTIES heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	//深度ステンシルビュー用リソースの生成
	createCommittedResource(device, heap_prop, D3D12_HEAP_FLAG_NONE, resouce_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsv_clear_value);

}

/// <summary>
/// ディスクリプタ作成
/// </summary>
/// <param name="device">GPUデバイス</param>
void DepthStencil::createDSV(ID3D12Device* device)
{
	//深度ステンシルビューの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
	dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;

	//深度ステンシルビューの生成
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(this->descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	device->CreateDepthStencilView(getResource(), &dsv_desc, dsvHandle);

}

ID3D12DescriptorHeap* DepthStencil::getDescriptorHeap() const
{
	return this->descriptor_heap_->getDescriptorHeap();
}
