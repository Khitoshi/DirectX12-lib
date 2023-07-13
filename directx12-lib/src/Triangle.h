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
        vertices(),
        renderMode(RenderMode::Default)
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

public:

    /// <summary>
    /// 描画モードの設定
    /// </summary>
    /// <param name="mode"></param>
    void setRenderMode(RenderMode mode) {
        renderMode = mode;
    };
public:
    //頂点データ
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

    //頂点データの設定
    void setVertices(Vertex vertices[3]) {
        this->vertices[0] = vertices[0];
        this->vertices[1] = vertices[1];
        this->vertices[2] = vertices[2];
        vertexBuffer->copy(this->vertices);
    };

public:

    /// <summary>
    /// 頂点データの取得
    /// </summary>
    /// <param name="index">インデックス番号</param>
    /// <returns>
    /// インデックス番号に合った頂点データ
    /// </returns>
    Vertex getVertices(const int index) {
        if (index < 0 || index >= 3) {
            throw std::out_of_range("index out of range");
        }
        return vertices[index];
    };

private:

    std::shared_ptr<RootSignature> rootSignature;                                   //ルートシグニチャ
    std::shared_ptr<Shader> vertexShader;                                           //頂点シェーダー
    std::shared_ptr<Shader> pixelShader;                                            //ピクセルシェーダー
    //std::shared_ptr<PipelineStateObject> pipelineStateObject;                                //パイプラインステートオブジェクト
    std::shared_ptr<PipelineStateObject> pipelineStateObject[(int)RenderMode::Num]; //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertexBuffer;                                     //頂点バッファ
    std::shared_ptr<IndexBuffer> indexBuffer;                                       //インデックスバッファ
    Vertex vertices[3];										                        //頂点データ
    RenderMode renderMode;														    //描画モード
};
