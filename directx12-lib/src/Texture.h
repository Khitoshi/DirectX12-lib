#pragma once

#include "d3dx12.h"

#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

/// <summary>
/// �e�N�X�`��
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
    //�e�N�X�`���ǂݍ���
    void Load(ID3D12Device* device, const char* texture_file_path);
    //�e�N�X�`�����\�[�X�̍쐬
    void CreateTextureResource(ID3D12Device* device, const char* texture_file_path);
    //�f�B�X�N���v�^�q�[�v�̍쐬
    void CreateTextureDescriptorHeap(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[�̍쐬
    void CreateShaderResourceView(ID3D12Device* device);

public:
    //�e�N�X�`�����\�[�X�̎擾
    ComPtr<ID3D12Resource> GetResource() const { return this->resource_; }
    //�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* GetDescriptorHeap() { return this->descriptor_heap_.Get(); }
private:
    ComPtr<ID3D12Resource> resource_;				//�e�N�X�`�����\�[�X
    ComPtr<ID3D12DescriptorHeap> descriptor_heap_;	//�f�B�X�N���v�^�q�[�v
    DirectX::TexMetadata meta_data_;					//�e�N�X�`���̃��^�f�[�^
};
