#pragma once

#include "d3dx12.h"
#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

class DescriptorHeap;
struct aiTexture;
/// <summary>
/// �e�N�X�`��
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
    //�e�N�X�`����ǂݍ��݁C���\�[�X���쐬
    void Load(ID3D12Device* device, const char* texture_file_path);
public:
    //�V�F�[�_�[���\�[�X�r���[�̍쐬
    void CreateShaderResourceView(ID3D12Device* device, DescriptorHeap* descriptor_heap, const UINT slot);
public:
    //�e�N�X�`�����\�[�X�̎擾
    ComPtr<ID3D12Resource> GetResource() const { return this->resource_; }

private:
    ComPtr<ID3D12Resource> resource_;	//�e�N�X�`�����\�[�X
    DirectX::TexMetadata meta_data_;    //�e�N�X�`���̃��^�f�[�^

};
