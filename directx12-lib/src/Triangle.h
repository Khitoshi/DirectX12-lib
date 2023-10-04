#pragma once

#include "d3d12.h"
#include "RenderMode.h"

#include <memory>
#include <DirectXMath.h>
#include <stdexcept>

class RootSignature;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class DepthStencil;
class OffScreenRenderTarget;
class RenderContext;

/// <summary>
/// �V���v���ȎO�p�`��`�悷��N���X
/// </summary>
class Triangle
{
public:
	Triangle() :
		root_signature_(),
		vertex_shader_(),
		pixel_shader_(),
		pso_(),
		vertex_buffer_(),
		index_buffer_(),
		vertices_(),
		render_mode_(RenderMode::Default),
		off_screen_render_target_()
	{};
	~Triangle() {};

	//����������
	void init(ID3D12Device* device);
	//�`�揈��
	void draw(RenderContext* rc);

private:
	//���[�g�V�O�l�`���̍쐬
	void initRootSignature(ID3D12Device* device);
	//�V�F�[�_�[�̃��[�h
	void loadShader();
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
	void initPipelineStateObject(ID3D12Device* device);
	//���_�o�b�t�@�̍쐬
	void initVertexBuffer(ID3D12Device* device);
	//�C���f�b�N�X�o�b�t�@�̍쐬
	void initIndexBuffer(ID3D12Device* device);
	//�I�t�X�N���[�������_�[�^�[�Q�b�g�̍쐬
	void initOffScreenRenderTarget(ID3D12Device* device);
	//�[�x�X�e���V���̍쐬
	void initDepthStencil(ID3D12Device* device);

public:

	/// <summary>
	/// �`�惂�[�h�̐ݒ�
	/// </summary>
	/// <param name="mode"></param>
	void setRenderMode(RenderMode mode) {
		this->render_mode_ = mode;
	};
public:
	//���_�f�[�^
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	//���_�f�[�^�̐ݒ�
	void setVertices(Vertex vertices[3]);

public:

	/// <summary>
	/// ���_�f�[�^�̎擾
	/// </summary>
	/// <param name="index">�C���f�b�N�X�ԍ�</param>
	/// <returns>
	/// �C���f�b�N�X�ԍ��ɍ��������_�f�[�^
	/// </returns>
	Vertex getVertices(const int index) {
		if (index < 0 || index >= 3) {
			throw std::out_of_range("index out of range");
		}
		return this->vertices_[index];
	};

private:

	std::shared_ptr<RootSignature> root_signature_;                                   //���[�g�V�O�j�`��
	std::shared_ptr<Shader> vertex_shader_;                                           //���_�V�F�[�_�[
	std::shared_ptr<Shader> pixel_shader_;                                            //�s�N�Z���V�F�[�_�[
	std::shared_ptr<PipelineStateObject> pso_[(int)RenderMode::Num]; //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	std::shared_ptr<VertexBuffer> vertex_buffer_;                                     //���_�o�b�t�@
	std::shared_ptr<IndexBuffer> index_buffer_;                                       //�C���f�b�N�X�o�b�t�@
	std::shared_ptr<DepthStencil> depth_stencil_;                                     //�[�x�X�e���V��
	Vertex vertices_[3];										                        //���_�f�[�^
	RenderMode render_mode_;														    //�`�惂�[�h
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;                   //�I�t�X�N���[�������_�[�^�[�Q�b�g
	ID3D12Device* device_;
};
