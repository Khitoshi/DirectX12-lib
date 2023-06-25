#pragma once

#include "Shader.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderContext.h"
#include<DirectXMath.h>

/// <summary>
/// シンプルな三角形を描画するクラスの初期化情報
/// </summary>
struct TriangleConf
{
    ID3D12Device* device;
    //UINT frameBufferCount;
    //UINT width;
    //UINT height;
};

/// <summary>
/// シンプルな三角形を描画するクラス
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

    //初期化処理
    void init(TriangleConf conf);
    //描画処理
    void draw(RenderContext* rc);

private:
    //ルートシグネチャの作成
    void initRootSignature(TriangleConf conf);

    //シェーダーのロード
    void loadShader();

    //パイプラインステートオブジェクトの作成
    void initPipelineStateObject(TriangleConf conf);

    //頂点バッファの作成
    void initVertexBuffer(TriangleConf conf);

    //インデックスバッファの作成
    void initIndexBuffer(TriangleConf conf);
private:
    //頂点データ
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };
    std::shared_ptr<RootSignature> rootSignature;               //ルートシグニチャ
    std::shared_ptr<Shader> vertexShader;                       //頂点シェーダー
    std::shared_ptr<Shader> pixelShader;                        //ピクセルシェーダー
    std::shared_ptr<PipelineStateObject> pipelineStateObject;   //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertexBuffer;                 //頂点バッファ
    std::shared_ptr<IndexBuffer> indexBuffer;                   //インデックスバッファ
    Vertex vertices[3];										    //頂点データ
};
