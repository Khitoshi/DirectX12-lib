#pragma once

#include "d3dx12.h"

#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

/// <summary>
/// テクスチャ
/// </summary>
class Texture
{
    friend class TextureFactory;
private:
    Texture() :
        resource_(),
        descriptor_heap_(),
        meta_data_()
    {}
public:
    ~Texture() {}

private:
    //テクスチャ読み込み
    void Load(ID3D12Device* device, const char* texture_file_path);
    //テクスチャリソースの作成
    void CreateTextureResource(ID3D12Device* device, const char* texture_file_path);
    //ディスクリプタヒープの作成
    void CreateTextureDescriptorHeap(ID3D12Device* device);
    //シェーダーリソースビューの作成
    void CreateShaderResourceView(ID3D12Device* device);

public:
    //テクスチャリソースの取得
    ComPtr<ID3D12Resource> GetResource() const { return this->resource_; }
    //ディスクリプタヒープの取得
    ID3D12DescriptorHeap* GetDescriptorHeap() { return this->descriptor_heap_.Get(); }
private:
    ComPtr<ID3D12Resource> resource_;				//テクスチャリソース
    ComPtr<ID3D12DescriptorHeap> descriptor_heap_;	//ディスクリプタヒープ
    DirectX::TexMetadata meta_data_;					//テクスチャのメタデータ
};
