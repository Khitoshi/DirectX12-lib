#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <memory>

class RenderContext;
class RootSignature;
class DescriptorHeap;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Texture;
class DepthStencil;
class OffScreenRenderTarget;


class TextureCubeModel
{
public:
    //�萔�o�b�t�@�ւ̃f�[�^�]���p�\����
    struct TextureCubeModelConf
    {
        DirectX::XMFLOAT4X4  model;
        DirectX::XMFLOAT4X4  view;
        DirectX::XMFLOAT4X4  projection;
    };
public:
    TextureCubeModel(const TextureCubeModelConf& c) :
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
        texture_(),
        depth_stencil_(),
        off_screen_render_target_()
    {};
    ~TextureCubeModel() {};

    void init(ID3D12Device* device, const char* texture_file_path);
    void update();
    void draw(RenderContext* rc);

private:
    void loadShader();
    void initRootSignature(ID3D12Device* device);
    void initDescriptorHeap(ID3D12Device* device);
    void initPipelineStateObject(ID3D12Device* device);
    void initVertexBuffer(ID3D12Device* device);
    void initIndexBuffer(ID3D12Device* device);
    void initConstantBuffer(ID3D12Device* device);
    void initTexture(ID3D12Device* device, const char* texture_file_path);
    void initDepthStencil(ID3D12Device* device);
    void initOffScreenRenderTarget(ID3D12Device* device);

public:
    void setConf(const TextureCubeModelConf& c) { this->conf_ = c; }
private:
    //���_�f�[�^
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
    };
    TextureCubeModelConf conf_;
    std::shared_ptr<RootSignature> root_signature_;                     //���[�g�V�O�j�`��
    std::shared_ptr<DescriptorHeap> srv_cbv_uav_descriptor_heap_;                  //�f�B�X�N���v�^�q�[�v
    std::shared_ptr<Shader> vertex_shader_;                             //���_�V�F�[�_�[
    std::shared_ptr<Shader> pixel_shader_;                              //�s�N�Z���V�F�[�_�[
    std::shared_ptr<PipelineStateObject> pso_;                          //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vertex_buffer_;                       //���_�o�b�t�@
    std::shared_ptr<IndexBuffer> index_buffer_;                         //�C���f�b�N�X�o�b�t�@
    std::shared_ptr<ConstantBuffer> constant_buffer_;                   //�萔�o�b�t�@
    std::shared_ptr<DepthStencil> depth_stencil_;                       //�[�x�X�e���V��
    Vertex vertices_[8];										        //���_�f�[�^
    UINT num_indices_;                                                  //�C���f�b�N�X��
    std::shared_ptr<Texture> texture_;
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;   //�I�t�X�N���[�������_�[�^�[�Q�b�g
};
