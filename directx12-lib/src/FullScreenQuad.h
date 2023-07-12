#pragma once

#include "PSOCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "ShaderCacheManager.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderContext.h"
#include "Texture.h"
#include "OffScreenRenderTarget.h"
#include <DirectXMath.h>

class FullScreenQuad
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};
public:
	FullScreenQuad() {};
	~FullScreenQuad() {};

	void init(ID3D12Device* device);
	void draw(RenderContext* rc, OffScreenRenderTarget* osrt);
private:
	//シェーダーのペア
	void createShader(ID3D12Device* device);
	//頂点バッファの作成
	void createVertexBuffer(ID3D12Device* device);
	//インデックスバッファの作成
	void createIndexBuffer(ID3D12Device* device);
	//ルートシグネチャの作成
	void createRootSignature(ID3D12Device* device);
	//パイプラインステートの作成
	void createPipelineState(ID3D12Device* device);
	//テクスチャの作成
	void createTexture(ID3D12Device* device);
private:
	std::shared_ptr<VertexBuffer> vertexBuffer;				//頂点バッファ
	std::shared_ptr<IndexBuffer> indexBuffer;				//インデックスバッファ
	std::shared_ptr<RootSignature> rootSignature;			//ルートシグネチャ
	std::shared_ptr<PipelineStateObject> pso;				//パイプラインステートオブジェクト
	BasicShaderPair basicShaderPair;						//シェーダーのペア
	std::shared_ptr<Texture> texture;						//テクスチャ
	int numIndices;											//インデックスの数
};
