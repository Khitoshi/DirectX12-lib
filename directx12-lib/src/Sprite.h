#pragma once
#include "d3dx12.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "RenderContext.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>
#include <DirectXMath.h>
#include <stdexcept>
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
		const char* filePath;
	};

public:
	Sprite():
		rootSignature(),
		vertexShader(),
		pixelShader(),
		pipelineStateObject(),
		vertexBuffer(),
		indexBuffer(),
		vertices(),
		numIndices(),
		texture()
	{};
	~Sprite() {};
	
	//初期化処理
	void init(const SpriteConf conf);
	//描画処理
	void draw(RenderContext* rc);

private:

	//ルートシグネチャの作成
	void initRootSignature(SpriteConf conf);
	//シェーダーのロード
	void loadShader();
	//パイプラインステートオブジェクトの作成
	void initPipelineStateObject(SpriteConf conf);
	//頂点バッファの作成
	void initVertexBuffer(SpriteConf conf);
	//インデックスバッファの作成
	void initIndexBuffer(SpriteConf conf);
	//テクスチャの作成
	void initTexture(SpriteConf conf);

public:
	//頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3 position;	//座標
		DirectX::XMFLOAT2 uv;		//テクスチャ座標
	};


	/// <summary>
	/// 頂点データの設定
	/// </summary>
	/// <param name="vertices">頂点情報</param>
	void setVertices(Vertex vertices[4]) {
		this->vertices[0] = vertices[0];
		this->vertices[1] = vertices[1];
		this->vertices[2] = vertices[2];
		this->vertices[3] = vertices[3];
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
		if (index < 0 || index >= 4) {
			throw std::out_of_range("index out of range");
		}
		return vertices[index];
	};

private:
	std::shared_ptr<RootSignature> rootSignature;                                   //ルートシグニチャ
	std::shared_ptr<Shader> vertexShader;                                           //頂点シェーダー
	std::shared_ptr<Shader> pixelShader;                                            //ピクセルシェーダー
	std::shared_ptr<PipelineStateObject> pipelineStateObject;                       //パイプラインステートオブジェクト
	//std::shared_ptr<PipelineStateObject> pipelineStateObject[(int)RenderMode::Num]; //パイプラインステートオブジェクト
	std::shared_ptr<VertexBuffer> vertexBuffer;                                     //頂点バッファ
	std::shared_ptr<IndexBuffer> indexBuffer;                                       //インデックスバッファ
	Vertex vertices[4];										                        //頂点データ
	unsigned int numIndices;                                                        //インデックス数
	
	std::shared_ptr<Texture> texture;												//テクスチャ
};

