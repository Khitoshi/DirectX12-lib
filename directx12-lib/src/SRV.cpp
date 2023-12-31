#include "SRV.h"
#include "DescriptorHeap.h"

void SRV::init(ID3D12Device* device)
{
	createCommittedResource(
		device,
		*this->heap_prop_,
		D3D12_HEAP_FLAG_NONE,
		*this->desc_,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr);

	setName(L"Shader Resouce View");
}

void SRV::createSRV(ID3D12Device* device, const DXGI_FORMAT& format, DescriptorHeap* descriptor_heap, const UINT slot)
{
	//通常テクスチャビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = format;//RGBA(0.0f〜1.0fに正規化)
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srv_desc.Texture2D.MipLevels = 1;
	auto handle = descriptor_heap->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * static_cast<unsigned long long>(slot);

	device->CreateShaderResourceView(getResource(), &srv_desc, handle);
}

void SRV::writeToSubresource(const UINT& DstSubresource, const void* src, const UINT& SrcRowPitch, const UINT& SrcDepthPitch)
{
	if (FAILED(getResource()->WriteToSubresource(DstSubresource, nullptr, src, SrcRowPitch, SrcDepthPitch))) {
		throw std::runtime_error("FAILED SRV::WriteToSubresource WriteToSubresource");
	}
}
