#pragma once

#include "Shader.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderContext.h"
#include<DirectXMath.h>
struct TriangleConf
{
	ID3D12Device* device;
    RootSignature* rootSignature;
	//UINT frameBufferCount;
	//UINT width;
	//UINT height;
};



class Triangle
{
public:
    Triangle(): 
        vertexShader(),
		pixelShader(),
		pipelineStateObject(),
		vertexBuffer(),
		indexBuffer(),
        vertices()
    {};
    ~Triangle() {};

    void init(TriangleConf conf);
    void draw(RenderContext* rc);
private:
    //�V�F�[�_�[�̃��[�h
    void loadShader();

    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
    void initPipelineStateObject(TriangleConf conf);

    void initVertexBuffer(TriangleConf conf);

    void initIndexBuffer(TriangleConf conf);
private:
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 color;
        DirectX::XMFLOAT2 texcoord;
    };

    std::shared_ptr<Shader> vertexShader;                       //���_�V�F�[�_�[
    std::shared_ptr<Shader> pixelShader;                        //�s�N�Z���V�F�[�_�[
    std::shared_ptr<PipelineStateObject> pipelineStateObject;   //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vertexBuffer;                 //���_�o�b�t�@
    std::shared_ptr<IndexBuffer> indexBuffer;                   //�C���f�b�N�X�o�b�t�@
    Vertex vertices[3];										    //���_�f�[�^

};
