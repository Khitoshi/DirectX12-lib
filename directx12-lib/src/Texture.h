#pragma once

#include "d3dx12.h"

#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

/// <summary>
/// テクスチャ生成クラス
/// </summary>
class Texture
{
public:
	struct TextureConf
	{
		ID3D12Device* device;
		const char* filePath;
	};
public:
	Texture(TextureConf c):
		conf(c),
		resource(),
		descriptorHeap(),
		metadata()
	{}
	~Texture() {}

	//テクスチャ読み込み
	void Load();
	
private:
	//テクスチャリソースの作成
	void CreateTextureResource();
	//ディスクリプタヒープの作成
	void CreateTextureDescriptorHeap();
	//シェーダーリソースビューの作成
	void CreateShaderResourceView();

public:
	ComPtr<ID3D12Resource> GetResource() const { return resource; }
	ID3D12DescriptorHeap* GetDescriptorHeap()  { return descriptorHeap.Get(); }
private:
	TextureConf conf;								//テクスチャ生成時に使用する設定
	ComPtr<ID3D12Resource> resource;				//テクスチャリソース
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;	//ディスクリプタヒープ
	DirectX::TexMetadata metadata;					//テクスチャのメタデータ
};
