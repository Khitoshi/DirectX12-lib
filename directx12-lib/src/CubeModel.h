#pragma once
#include "d3dx12.h"
#include <memory>
#include <DirectXMath.h>

class RootSignature;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class DepthStencil;
class OffScreenRenderTarget;

class RenderContext;

class CubeModel
{
public:
    //�萔�o�b�t�@�ւ̃f�[�^�]���p�\����
    struct CubeModelConf
    {
        DirectX::XMFLOAT4X4  model;
        DirectX::XMFLOAT4X4  view;
        DirectX::XMFLOAT4X4  projection;
    };

public:
    CubeModel(const CubeModelConf c) :
        conf_(c),
        root_signature_(),
        vertex_shader_(),
        pixel_shader_(),
        pso_(),
        vertex_buffer_(),
        index_buffer_(),
        constant_buffer_(),
        vertices_(),
        num_indices_(0),
        off_screen_render_target_()
    {};
    ~CubeModel() {};

    //����������
    void init(ID3D12Device* device);
    //�X�V����
    void update();
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
    //�萔�o�b�t�@�̍쐬
    void initConstantBuffer(ID3D12Device* device);
    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̍쐬
    void initOffScreenRenderTarget(ID3D12Device* device);
    //�[�x�X�e���V���̍쐬
    void initDepthStencil(ID3D12Device* device);

public:
    void setConf(const CubeModelConf c) { conf_ = c; }
private:
    //���_�f�[�^
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
    };

    CubeModelConf conf_;
    std::shared_ptr<RootSignature> root_signature_;                     //���[�g�V�O�j�`��
    std::shared_ptr<Shader> vertex_shader_;                             //���_�V�F�[�_�[
    std::shared_ptr<Shader> pixel_shader_;                              //�s�N�Z���V�F�[�_�[
    std::shared_ptr<PipelineStateObject> pso_;                          //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vertex_buffer_;                       //���_�o�b�t�@
    std::shared_ptr<IndexBuffer> index_buffer_;                         //�C���f�b�N�X�o�b�t�@
    std::shared_ptr<ConstantBuffer> constant_buffer_;                   //�萔�o�b�t�@
    std::shared_ptr<DepthStencil> depth_stencil_;                       //�[�x�X�e���V��
    Vertex vertices_[8];										        //���_�f�[�^
    UINT num_indices_;                                                  //�C���f�b�N�X��
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;   //�I�t�X�N���[�������_�[�^�[�Q�b�g

};