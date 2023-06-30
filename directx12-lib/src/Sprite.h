#pragma once
#include "d3dx12.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "RenderContext.h"
#include "Shader.h"
#include "Texture.h"
#include "Rotation.h"
#include <memory>
#include <DirectXMath.h>
#include <stdexcept>
#include <map>
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
		defaultShaderPair(),
		defaultPipelineStateObject(),
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

	//デフォルトパイプラインステートオブジェクトの作成
	void initDefaultPipelineStateObject(SpriteConf conf);

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
	enum class SpriteOptions : uint8_t {
		None = 1 << 0, // 1
		Rotated = 1 << 1, // 2
		// 他のオプション...

		Num,
	};

	std::shared_ptr<RootSignature> rootSignature;                                   //ルートシグニチャ
	BasicShaderPair defaultShaderPair;												//デフォルトのシェーダー
	std::shared_ptr<PipelineStateObject> defaultPipelineStateObject;                       //パイプラインステートオブジェクト
	std::shared_ptr<VertexBuffer> vertexBuffer;                                     //頂点バッファ
	std::shared_ptr<IndexBuffer> indexBuffer;                                       //インデックスバッファ
	Vertex vertices[4];										                        //頂点データ
	unsigned int numIndices;                                                        //インデックス数
	std::shared_ptr<Texture> texture;												//テクスチャ

	std::shared_ptr<Rotation> rotation;												//回転
};


