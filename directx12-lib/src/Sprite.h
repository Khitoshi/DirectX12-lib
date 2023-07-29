#pragma once
#include "d3dx12.h"

#include <memory>
#include <DirectXMath.h>
#include <stdexcept>
#include <map>

class RootSignature;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class Texture;
class Camera;
class RenderContext;
class Rotation;
class RotationEffect;
class OffScreenRenderTarget;
class Shader;
class DepthStencil;

using namespace Microsoft::WRL;

/// <summary>
/// �摜�`��p�̃N���X
/// </summary>
class Sprite
{
public:
    struct SpriteConf
    {
        const char* file_path;
        Camera* camera;
    };

public:
    Sprite(SpriteConf c) :
        conf_(c),
        root_signature_(nullptr),
        pixel_shader_(nullptr),
        vertex_shader_(nullptr),
        pso_(nullptr),
        vertex_buffer_(nullptr),
        index_buffer_(nullptr),
        num_indices_(0),
        vertices_(),
        texture_(nullptr),
        off_screen_render_target_(nullptr)
    {};
    ~Sprite() {};

    //����������
    void init(ID3D12Device* device);
    //�`�揈��
    void draw(RenderContext* rc);

private:

    //���[�g�V�O�l�`���̍쐬
    void initRootSignature(ID3D12Device* device);
    //�V�F�[�_�[�̃��[�h
    void initShader();
    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
    void initPipelineStateObject(ID3D12Device* device);

    //���_�o�b�t�@�̍쐬
    void initVertexBuffer(ID3D12Device* device);
    //�C���f�b�N�X�o�b�t�@�̍쐬
    void initIndexBuffer(ID3D12Device* device);
    //�e�N�X�`���̍쐬
    void initTexture(ID3D12Device* device);
    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̍쐬
    void initOffScreenRenderTarget(ID3D12Device* device);
    //�[�x�X�e���V���̍쐬
    void initDepthStencil(ID3D12Device* device);
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
    void setVertices(Vertex vertices[4]);

public://�擾�n
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
        return vertices_[index];
    };

private:
    enum class SpriteOptions : uint8_t {
        NONE = 1 << 0, // 1
        ROTATED = 1 << 1, // 2
        // ���̃I�v�V����...

        NUM,
    };

    SpriteConf conf_;
    std::shared_ptr<RootSignature> root_signature_;                     //���[�g�V�O�j�`��
    std::shared_ptr<Shader> pixel_shader_;                              //�s�N�Z���V�F�[�_�[
    std::shared_ptr<Shader> vertex_shader_;                             //���_�V�F�[�_�[
    std::shared_ptr<PipelineStateObject> pso_;                          //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vertex_buffer_;                       //���_�o�b�t�@
    std::shared_ptr<IndexBuffer> index_buffer_;                         //�C���f�b�N�X�o�b�t�@
    Vertex vertices_[4];										        //���_�f�[�^
    unsigned int num_indices_;                                          //�C���f�b�N�X��
    std::shared_ptr<Texture> texture_;									//�e�N�X�`��
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;   //�I�t�X�N���[�������_�[�^�[�Q�b�g
    std::shared_ptr< DepthStencil> depth_stencil_;                      //�[�x�X�e���V��
};
