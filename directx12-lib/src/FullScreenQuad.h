#pragma once
#include "d3dx12.h"
#include <memory>
#include <DirectXMath.h>

class VertexBuffer;
class IndexBuffer;
class RootSignature;
class DescriptorHeap;
class PipelineStateObject;
class Shader;
class RenderContext;
class Texture;
class CompositeRenderTarget;


using namespace Microsoft::WRL;

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g���t���X�N���[���ɕ`�悷��N���X
/// </summary>
class FullScreenQuad
{
    friend class FullScreenQuadFactory;
public:
    //���_���\����
    struct Vertex
    {
        DirectX::XMFLOAT3 position;	//���_���W
        DirectX::XMFLOAT2 uv;		//�e�N�X�`�����W
    };

private:
    FullScreenQuad() :
        vertex_buffer_(),
        index_buffer_(),
        root_signature_(),
        pso_(),
        pixel_shader_(),
        vertex_shader_(),
        texture_(),
        num_indices_(0),
        cbv_srv_descriptor_heap_()
    {};

public:
    ~FullScreenQuad() {};
    //�`�揈��
    void draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* osrt);

private:
    //����������
    void init(ID3D12Device* device);
    //�V�F�[�_�[�̃y�A
    void createShader(ID3D12Device* device);
    //���_�o�b�t�@�̍쐬
    void createVertexBuffer(ID3D12Device* device);
    //���[�g�V�O�l�`���̍쐬
    void createRootSignature(ID3D12Device* device);
    //�p�C�v���C���X�e�[�g�̍쐬
    void createPipelineState(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[�q�[�v�̍쐬
    void createSRVHeap(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[�̍쐬
    void createSRV(ID3D12Device* device, CompositeRenderTarget* osrt);

private:
    std::shared_ptr<VertexBuffer> vertex_buffer_;	//���_�o�b�t�@
    std::shared_ptr<IndexBuffer> index_buffer_;		//�C���f�b�N�X�o�b�t�@
    std::shared_ptr<RootSignature> root_signature_; //���[�g�V�O�l�`��
    std::shared_ptr<PipelineStateObject> pso_;		//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<Shader> pixel_shader_;			//�s�N�Z���V�F�[�_�[
    std::shared_ptr<Shader> vertex_shader_;			//�o�[�e�b�N�X�V�F�[�_�[
    std::shared_ptr<Texture> texture_;				//�e�N�X�`��
    UINT num_indices_;								//�C���f�b�N�X�̐�
    std::shared_ptr<DescriptorHeap> cbv_srv_descriptor_heap_;  //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v
};
