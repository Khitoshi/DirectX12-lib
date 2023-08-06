#pragma once

#include "d3dx12.h"

#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

class DescriptorHeap;

/// <summary>
/// テクスチャ
/// </summary>
class Texture
{
    friend class TextureFactory;
private:
    Texture() :
        resource_(),
        meta_data_()
    {}
public:
    ~Texture() {}

private:
    //テクスチャ読み込み
    void Load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* texture_file_path);
    //テクスチャリソースの作成
    void CreateTextureResource(ID3D12Device* device, const char* texture_file_path);
    //シェーダーリソースビューの作成
    void CreateShaderResourceView(ID3D12Device* device, DescriptorHeap* descriptor_heap);

public:
    //テクスチャリソースの取得
    ComPtr<ID3D12Resource> GetResource() const { return this->resource_; }

private:
    ComPtr<ID3D12Resource> resource_;	//テクスチャリソース
    DirectX::TexMetadata meta_data_;    //テクスチャのメタデータ
};
