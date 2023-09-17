#pragma once

#include "d3dx12.h"
#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")



class DescriptorHeap;
struct aiTexture;
class SRV;

/// <summary>
/// テクスチャ
/// </summary>
class Texture
{
	friend class TextureFactory;
private:
	Texture() :
		srv_(),
		meta_data_()
	{}
public:
	~Texture() {}

private:
	void Load(ID3D12Device* device, const char* texture_file_path);

public:
	//void CreateShaderResourceView(ID3D12Device* device, DescriptorHeap* descriptor_heap, const UINT slot);
	void createSRV(ID3D12Device* device, DescriptorHeap* descriptor_heap, const UINT slot);
private:
	std::shared_ptr<SRV> srv_;
	DirectX::TexMetadata meta_data_;
};
