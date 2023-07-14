#pragma once

#include "PSOCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "ShaderCacheManager.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderContext.h"
#include "Texture.h"
#include "CompositeRenderTarget.h"
#include <DirectXMath.h>

/// <summary>
/// オフスクリーンレンダーターゲットをフルスクリーンに描画するクラス
/// </summary>
class FullScreenQuad
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;	//頂点座標
		DirectX::XMFLOAT2 uv;		//テクスチャ座標
	};
public:
	FullScreenQuad():
		vertexBuffer(),
		indexBuffer(),
		rootSignature(),
		pso(),
		basicShaderPair(),
		texture(),
		numIndices(0)
	{};
	~FullScreenQuad() {};

	//初期化処理
	void init(ID3D12Device* device);
	//描画処理
	void draw(RenderContext* rc, CompositeRenderTarget* osrt);
private:
	//シェーダーのペア
	void createShader(ID3D12Device* device);
	//頂点バッファの作成
	void createVertexBuffer(ID3D12Device* device);
	//ルートシグネチャの作成
	void createRootSignature(ID3D12Device* device);
	//パイプラインステートの作成
	void createPipelineState(ID3D12Device* device);
private:
	std::shared_ptr<VertexBuffer> vertexBuffer;				//頂点バッファ
	std::shared_ptr<IndexBuffer> indexBuffer;				//インデックスバッファ
	std::shared_ptr<RootSignature> rootSignature;			//ルートシグネチャ
	std::shared_ptr<PipelineStateObject> pso;				//パイプラインステートオブジェクト
	BasicShaderPair basicShaderPair;						//シェーダーのペア
	std::shared_ptr<Texture> texture;						//テクスチャ
	int numIndices;											//インデックスの数
};
