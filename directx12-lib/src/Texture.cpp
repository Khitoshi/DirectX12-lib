#include "Texture.h"
#include "DescriptorHeap.h"
#include <stdexcept>
#include <assimp/texture.h>
#include "SRV.h"
#include "SRVFactory.h"


void Texture::Load(ID3D12Device* device, const char* texture_file_path)
{
	wchar_t w_file_Path[256];
	MultiByteToWideChar(CP_UTF8, 0, texture_file_path, -1, w_file_Path, _countof(w_file_Path));

	DirectX::ScratchImage scratch_img = {};
	if (FAILED(LoadFromWICFile(w_file_Path, DirectX::WIC_FLAGS_NONE, &this->meta_data_, scratch_img))) {
		throw std::runtime_error("failed to load texture");
	}

	//生データ抽出
	auto img = scratch_img.GetImage(0, 0, 0);

	//WriteToSubresourceで転送する用のヒープ設定
	D3D12_HEAP_PROPERTIES tex_heap_prop = {};
	tex_heap_prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	tex_heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	tex_heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	tex_heap_prop.CreationNodeMask = 0;
	tex_heap_prop.VisibleNodeMask = 0;

	//テクスチャの設定
	D3D12_RESOURCE_DESC tex_desc = {};
	tex_desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(this->meta_data_.dimension);
	tex_desc.Alignment = 0;
	tex_desc.Width = this->meta_data_.width;
	tex_desc.Height = static_cast<UINT>(this->meta_data_.height);
	tex_desc.DepthOrArraySize = static_cast<UINT16>(this->meta_data_.arraySize);
	tex_desc.MipLevels = static_cast<UINT16>(this->meta_data_.mipLevels);
	tex_desc.Format = this->meta_data_.format;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	tex_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	this->srv_ = SRVFactory::create(device, &tex_heap_prop, &tex_desc);

	this->srv_->writeToSubresource(
		0,
		img->pixels,
		static_cast<UINT>(img->rowPitch),
		static_cast<UINT>(img->slicePitch));
}

void Texture::createSRV(ID3D12Device* device, DescriptorHeap* descriptor_heap, const UINT slot)
{
	this->srv_->createSRV(device, this->meta_data_.format, descriptor_heap, slot);
}
