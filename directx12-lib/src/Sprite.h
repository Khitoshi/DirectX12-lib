#pragma once
#include "d3dx12.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "RenderContext.h"
#include "Shader.h"
#include <memory>
#include <DirectXMath.h>
using namespace Microsoft::WRL;

/// <summary>
/// 画像描画用のクラス
/// </summary>
class Sprite
{
public:
	struct SpriteConf
	{
		ID3D12Device* device;
	};
public:
	Sprite() {};
	~Sprite() {};
	
	void init(const SpriteConf conf);
	void draw(RenderContext* rc);

private:

	void initRootSignature(SpriteConf conf);
	void loadShader();
	void initPipelineStateObject(SpriteConf conf);
	void initVertexBuffer(SpriteConf conf);
	void initIndexBuffer(SpriteConf conf);

private:
	//頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	std::shared_ptr<RootSignature> rootSignature;                                   //ルートシグニチャ
	std::shared_ptr<Shader> vertexShader;                                           //頂点シェーダー
	std::shared_ptr<Shader> pixelShader;                                            //ピクセルシェーダー
	std::shared_ptr<PipelineStateObject> pipelineStateObject;                                //パイプラインステートオブジェクト
	//std::shared_ptr<PipelineStateObject> pipelineStateObject[(int)RenderMode::Num]; //パイプラインステートオブジェクト
	std::shared_ptr<VertexBuffer> vertexBuffer;                                     //頂点バッファ
	std::shared_ptr<IndexBuffer> indexBuffer;                                       //インデックスバッファ
	Vertex vertices[4];										                        //頂点データ
	unsigned int numIndices;                                                                 //インデックス数
	
};

