#pragma once

#include "d3dx12.h"

#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

class DescriptorHeap;

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
    //�e�N�X�`���ǂݍ���
    void Load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* texture_file_path);
    //�e�N�X�`�����\�[�X�̍쐬
    void CreateTextureResource(ID3D12Device* device, const char* texture_file_path);
    //�V�F�[�_�[���\�[�X�r���[�̍쐬
    void CreateShaderResourceView(ID3D12Device* device, DescriptorHeap* descriptor_heap);

public:
    //�e�N�X�`�����\�[�X�̎擾
    ComPtr<ID3D12Resource> GetResource() const { return this->resource_; }

private:
    ComPtr<ID3D12Resource> resource_;	//�e�N�X�`�����\�[�X
    DirectX::TexMetadata meta_data_;    //�e�N�X�`���̃��^�f�[�^
};
