#pragma once

#include "d3dx12.h"

#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

using namespace Microsoft::WRL;

/// <summary>
/// �e�N�X�`�������N���X
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

	//�e�N�X�`���ǂݍ���
	void Load();
	
private:
	//�e�N�X�`�����\�[�X�̍쐬
	void CreateTextureResource();
	//�f�B�X�N���v�^�q�[�v�̍쐬
	void CreateTextureDescriptorHeap();
	//�V�F�[�_�[���\�[�X�r���[�̍쐬
	void CreateShaderResourceView();

public:
	ComPtr<ID3D12Resource> GetResource() const { return resource; }
	ID3D12DescriptorHeap* GetDescriptorHeap()  { return descriptorHeap.Get(); }
private:
	TextureConf conf;								//�e�N�X�`���������Ɏg�p����ݒ�
	ComPtr<ID3D12Resource> resource;				//�e�N�X�`�����\�[�X
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;	//�f�B�X�N���v�^�q�[�v
	DirectX::TexMetadata metadata;					//�e�N�X�`���̃��^�f�[�^
};
