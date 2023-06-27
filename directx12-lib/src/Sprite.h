#pragma once
#include "d3dx12.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "RenderContext.h"
#include "Shader.h"
#include <memory>
#include <DirectXMath.h>
using namespace Microsoft::WRL;

/// <summary>
/// �摜�`��p�̃N���X
/// </summary>
class Sprite
{
public:
	struct SpriteConf
	{
		ID3D12Device* device;
	};
public:
	Sprite() {};
	~Sprite() {};
	
	void init(const SpriteConf conf);
	void draw(RenderContext* rc);

private:

	void initRootSignature(SpriteConf conf);
	void loadShader();
	void initPipelineStateObject(SpriteConf conf);
	void initVertexBuffer(SpriteConf conf);
	void initIndexBuffer(SpriteConf conf);

private:
	//���_�f�[�^
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	std::shared_ptr<RootSignature> rootSignature;                                   //���[�g�V�O�j�`��
	std::shared_ptr<Shader> vertexShader;                                           //���_�V�F�[�_�[
	std::shared_ptr<Shader> pixelShader;                                            //�s�N�Z���V�F�[�_�[
	std::shared_ptr<PipelineStateObject> pipelineStateObject;                                //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	//std::shared_ptr<PipelineStateObject> pipelineStateObject[(int)RenderMode::Num]; //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	std::shared_ptr<VertexBuffer> vertexBuffer;                                     //���_�o�b�t�@
	std::shared_ptr<IndexBuffer> indexBuffer;                                       //�C���f�b�N�X�o�b�t�@
	Vertex vertices[4];										                        //���_�f�[�^
	unsigned int numIndices;                                                                 //�C���f�b�N�X��
	
};

