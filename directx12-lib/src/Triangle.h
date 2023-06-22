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
    //シェーダーのロード
    void loadShader();

    //パイプラインステートオブジェクトの作成
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

    std::shared_ptr<Shader> vertexShader;                       //頂点シェーダー
    std::shared_ptr<Shader> pixelShader;                        //ピクセルシェーダー
    std::shared_ptr<PipelineStateObject> pipelineStateObject;   //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertexBuffer;                 //頂点バッファ
    std::shared_ptr<IndexBuffer> indexBuffer;                   //インデックスバッファ
    Vertex vertices[3];										    //頂点データ

};
