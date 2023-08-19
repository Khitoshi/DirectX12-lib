#pragma once

#include "d3dx12.h"
#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

class DescriptorHeap;
struct aiTexture;
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
    //テクスチャを読み込み，リソースを作成
    void Load(ID3D12Device* device, const char* texture_file_path);
public:
    //シェーダーリソースビューの作成
    void CreateShaderResourceView(ID3D12Device* device, DescriptorHeap* descriptor_heap, const UINT slot);
public:
    //テクスチャリソースの取得
    ComPtr<ID3D12Resource> GetResource() const { return this->resource_; }

private:
    ComPtr<ID3D12Resource> resource_;	//テクスチャリソース
    DirectX::TexMetadata meta_data_;    //テクスチャのメタデータ

};
