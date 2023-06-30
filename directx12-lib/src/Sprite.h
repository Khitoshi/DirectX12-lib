#pragma once
#include "d3dx12.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "RenderContext.h"
#include "Shader.h"
#include "Texture.h"
#include "Rotation.h"
#include <memory>
#include <DirectXMath.h>
#include <stdexcept>
#include <map>
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
		const char* filePath;
	};

public:
	Sprite():
		rootSignature(),
		defaultShaderPair(),
		defaultPipelineStateObject(),
		vertexBuffer(),
		indexBuffer(),
		vertices(),
		numIndices(),
		texture()
	{};
	~Sprite() {};
	
	//����������
	void init(const SpriteConf conf);
	//�`�揈��
	void draw(RenderContext* rc);

private:

	//���[�g�V�O�l�`���̍쐬
	void initRootSignature(SpriteConf conf);
	//�V�F�[�_�[�̃��[�h
	void loadShader();
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
	void initPipelineStateObject(SpriteConf conf);

	//�f�t�H���g�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
	void initDefaultPipelineStateObject(SpriteConf conf);

	//���_�o�b�t�@�̍쐬
	void initVertexBuffer(SpriteConf conf);
	//�C���f�b�N�X�o�b�t�@�̍쐬
	void initIndexBuffer(SpriteConf conf);
	//�e�N�X�`���̍쐬
	void initTexture(SpriteConf conf);

public:
	//���_�f�[�^
	struct Vertex
	{
		DirectX::XMFLOAT3 position;	//���W
		DirectX::XMFLOAT2 uv;		//�e�N�X�`�����W
	};


	/// <summary>
	/// ���_�f�[�^�̐ݒ�
	/// </summary>
	/// <param name="vertices">���_���</param>
	void setVertices(Vertex vertices[4]) {
		this->vertices[0] = vertices[0];
		this->vertices[1] = vertices[1];
		this->vertices[2] = vertices[2];
		this->vertices[3] = vertices[3];
		vertexBuffer->copy(this->vertices);
	};

public:

	/// <summary>
	/// ���_�f�[�^�̎擾
	/// </summary>
	/// <param name="index">�C���f�b�N�X�ԍ�</param>
	/// <returns>
	/// �C���f�b�N�X�ԍ��ɍ��������_�f�[�^
	/// </returns>
	Vertex getVertices(const int index) {
		if (index < 0 || index >= 4) {
			throw std::out_of_range("index out of range");
		}
		return vertices[index];
	};

private:
	enum class SpriteOptions : uint8_t {
		None = 1 << 0, // 1
		Rotated = 1 << 1, // 2
		// ���̃I�v�V����...

		Num,
	};

	std::shared_ptr<RootSignature> rootSignature;                                   //���[�g�V�O�j�`��
	BasicShaderPair defaultShaderPair;												//�f�t�H���g�̃V�F�[�_�[
	std::shared_ptr<PipelineStateObject> defaultPipelineStateObject;                       //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	std::shared_ptr<VertexBuffer> vertexBuffer;                                     //���_�o�b�t�@
	std::shared_ptr<IndexBuffer> indexBuffer;                                       //�C���f�b�N�X�o�b�t�@
	Vertex vertices[4];										                        //���_�f�[�^
	unsigned int numIndices;                                                        //�C���f�b�N�X��
	std::shared_ptr<Texture> texture;												//�e�N�X�`��

	std::shared_ptr<Rotation> rotation;												//��]
};


