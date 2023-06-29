#include "Texture.h"

#include <stdexcept>

/// <summary>
/// テクスチャ読み込み
/// </summary>
void Texture::Load()
{
	
	CreateTextureResource();
	CreateTextureDescriptorHeap();
	CreateShaderResourceView();	
}

/// <summary>
/// テクスチャリソースの作成
/// </summary>
void Texture::CreateTextureResource()
{
	//ファイルパスをワイド文字に変換
	wchar_t wFilePath[256];
	MultiByteToWideChar(CP_ACP, 0, conf.filePath, -1, wFilePath, _countof(wFilePath));

	//DirectXTexを使ってテクスチャを読み込む
	//DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImg = {};
	if (FAILED(LoadFromWICFile(wFilePath, DirectX::WIC_FLAGS_NONE, &this->metadata, scratchImg))) {
		throw std::runtime_error("failed to load texture");
	}


	//生データ抽出
	auto img = scratchImg.GetImage(0, 0, 0);

	//WriteToSubresourceで転送する用のヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	//テクスチャの設定
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(this->metadata.dimension);
	texDesc.Alignment = 0;
	texDesc.Width = this->metadata.width;
	texDesc.Height = static_cast<UINT>(this->metadata.height);
	texDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	texDesc.MipLevels = static_cast<UINT16>(this->metadata.mipLevels);
	texDesc.Format = this->metadata.format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//テクスチャの作成
	if (FAILED(conf.device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&this->resource)))) {
		throw std::runtime_error("failed to create texture");
	}

	//テクスチャの転送
	if (FAILED(this->resource->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		static_cast<UINT>(img->rowPitch),
		static_cast<UINT>(img->slicePitch)))) {
		throw std::runtime_error("failed to write texture");
	}
}

/// <summary>
/// ディスクリプタヒープの作成
/// </summary>
void Texture::CreateTextureDescriptorHeap()
{
	//ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//ディスクリプタヒープの作成
	if (FAILED(conf.device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&this->descriptorHeap)))) {
		throw std::runtime_error("failed to create descriptor heap");
	}
}

/// <summary>
/// シェーダーリソースビューの作成
/// </summary>
void Texture::CreateShaderResourceView()
{
	//通常テクスチャビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = this->metadata.format;//RGBA(0.0f～1.0fに正規化)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	conf.device->CreateShaderResourceView(this->resource.Get(), &srvDesc, this->descriptorHeap->GetCPUDescriptorHandleForHeapStart());
}
