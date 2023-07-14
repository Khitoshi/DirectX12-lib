#pragma once

#include "PSOCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "ShaderCacheManager.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderContext.h"
#include "Texture.h"
#include "CompositeRenderTarget.h"
#include <DirectXMath.h>

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g���t���X�N���[���ɕ`�悷��N���X
/// </summary>
class FullScreenQuad
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;	//���_���W
		DirectX::XMFLOAT2 uv;		//�e�N�X�`�����W
	};
public:
	FullScreenQuad():
		vertexBuffer(),
		indexBuffer(),
		rootSignature(),
		pso(),
		basicShaderPair(),
		texture(),
		numIndices(0)
	{};
	~FullScreenQuad() {};

	//����������
	void init(ID3D12Device* device);
	//�`�揈��
	void draw(RenderContext* rc, CompositeRenderTarget* osrt);
private:
	//�V�F�[�_�[�̃y�A
	void createShader(ID3D12Device* device);
	//���_�o�b�t�@�̍쐬
	void createVertexBuffer(ID3D12Device* device);
	//���[�g�V�O�l�`���̍쐬
	void createRootSignature(ID3D12Device* device);
	//�p�C�v���C���X�e�[�g�̍쐬
	void createPipelineState(ID3D12Device* device);
private:
	std::shared_ptr<VertexBuffer> vertexBuffer;				//���_�o�b�t�@
	std::shared_ptr<IndexBuffer> indexBuffer;				//�C���f�b�N�X�o�b�t�@
	std::shared_ptr<RootSignature> rootSignature;			//���[�g�V�O�l�`��
	std::shared_ptr<PipelineStateObject> pso;				//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	BasicShaderPair basicShaderPair;						//�V�F�[�_�[�̃y�A
	std::shared_ptr<Texture> texture;						//�e�N�X�`��
	int numIndices;											//�C���f�b�N�X�̐�
};
