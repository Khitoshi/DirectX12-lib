#pragma once

#include "Shader.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderContext.h"
#include<DirectXMath.h>
#include "RenderMode.h"
#include <stdexcept>
#include "OffScreenRenderTarget.h"
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
        vertices(),
        renderMode(RenderMode::Default)
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

public:

    /// <summary>
    /// �`�惂�[�h�̐ݒ�
    /// </summary>
    /// <param name="mode"></param>
    void setRenderMode(RenderMode mode) {
        renderMode = mode;
    };
public:
    //���_�f�[�^
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

    //���_�f�[�^�̐ݒ�
    void setVertices(Vertex vertices[3]) {
        this->vertices[0] = vertices[0];
        this->vertices[1] = vertices[1];
        this->vertices[2] = vertices[2];
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
        if (index < 0 || index >= 3) {
            throw std::out_of_range("index out of range");
        }
        return vertices[index];
    };

private:

    std::shared_ptr<RootSignature> rootSignature;                                   //���[�g�V�O�j�`��
    std::shared_ptr<Shader> vertexShader;                                           //���_�V�F�[�_�[
    std::shared_ptr<Shader> pixelShader;                                            //�s�N�Z���V�F�[�_�[
    //std::shared_ptr<PipelineStateObject> pipelineStateObject;                                //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<PipelineStateObject> pipelineStateObject[(int)RenderMode::Num]; //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vertexBuffer;                                     //���_�o�b�t�@
    std::shared_ptr<IndexBuffer> indexBuffer;                                       //�C���f�b�N�X�o�b�t�@
    Vertex vertices[3];										                        //���_�f�[�^
    RenderMode renderMode;														    //�`�惂�[�h
};
