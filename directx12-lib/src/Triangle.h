#pragma once

#include "Shader.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderContext.h"
#include<DirectXMath.h>

/// <summary>
/// �V���v���ȎO�p�`��`�悷��N���X�̏��������
/// </summary>
struct TriangleConf
{
    ID3D12Device* device;
    //UINT frameBufferCount;
    //UINT width;
    //UINT height;
};

/// <summary>
/// �V���v���ȎO�p�`��`�悷��N���X
/// </summary>
class Triangle
{
public:
    Triangle() :
        rootSignature(),
        vertexShader(),
        pixelShader(),
        pipelineStateObject(),
        vertexBuffer(),
        indexBuffer(),
        vertices()
    {};
    ~Triangle() {};

    //����������
    void init(TriangleConf conf);
    //�`�揈��
    void draw(RenderContext* rc);

private:
    //���[�g�V�O�l�`���̍쐬
    void initRootSignature(TriangleConf conf);

    //�V�F�[�_�[�̃��[�h
    void loadShader();

    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
    void initPipelineStateObject(TriangleConf conf);

    //���_�o�b�t�@�̍쐬
    void initVertexBuffer(TriangleConf conf);

    //�C���f�b�N�X�o�b�t�@�̍쐬
    void initIndexBuffer(TriangleConf conf);
private:
    //���_�f�[�^
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };
    std::shared_ptr<RootSignature> rootSignature;               //���[�g�V�O�j�`��
    std::shared_ptr<Shader> vertexShader;                       //���_�V�F�[�_�[
    std::shared_ptr<Shader> pixelShader;                        //�s�N�Z���V�F�[�_�[
    std::shared_ptr<PipelineStateObject> pipelineStateObject;   //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vertexBuffer;                 //���_�o�b�t�@
    std::shared_ptr<IndexBuffer> indexBuffer;                   //�C���f�b�N�X�o�b�t�@
    Vertex vertices[3];										    //���_�f�[�^
};
