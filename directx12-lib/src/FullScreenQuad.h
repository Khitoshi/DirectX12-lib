#pragma once

#include "PSOCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "ShaderCacheManager.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderContext.h"
#include "Texture.h"
#include "OffScreenRenderTarget.h"
#include <DirectXMath.h>

class FullScreenQuad
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};
public:
	FullScreenQuad() {};
	~FullScreenQuad() {};

	void init(ID3D12Device* device);
	void draw(RenderContext* rc, OffScreenRenderTarget* osrt);
private:
	//�V�F�[�_�[�̃y�A
	void createShader(ID3D12Device* device);
	//���_�o�b�t�@�̍쐬
	void createVertexBuffer(ID3D12Device* device);
	//�C���f�b�N�X�o�b�t�@�̍쐬
	void createIndexBuffer(ID3D12Device* device);
	//���[�g�V�O�l�`���̍쐬
	void createRootSignature(ID3D12Device* device);
	//�p�C�v���C���X�e�[�g�̍쐬
	void createPipelineState(ID3D12Device* device);
	//�e�N�X�`���̍쐬
	void createTexture(ID3D12Device* device);
private:
	std::shared_ptr<VertexBuffer> vertexBuffer;				//���_�o�b�t�@
	std::shared_ptr<IndexBuffer> indexBuffer;				//�C���f�b�N�X�o�b�t�@
	std::shared_ptr<RootSignature> rootSignature;			//���[�g�V�O�l�`��
	std::shared_ptr<PipelineStateObject> pso;				//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	BasicShaderPair basicShaderPair;						//�V�F�[�_�[�̃y�A
	std::shared_ptr<Texture> texture;						//�e�N�X�`��
	int numIndices;											//�C���f�b�N�X�̐�
};
