#pragma once

#include "d3d12.h"
#include "RenderMode.h"

#include <memory>
#include <DirectXMath.h>
#include <stdexcept>

class RootSignature;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class DepthStencil;
class OffScreenRenderTarget;
class RenderContext;

/// <summary>
/// シンプルな三角形を描画するクラス
/// </summary>
class Triangle
{
public:
	Triangle() :
		root_signature_(),
		vertex_shader_(),
		pixel_shader_(),
		pso_(),
		vertex_buffer_(),
		index_buffer_(),
		vertices_(),
		render_mode_(RenderMode::Default),
		off_screen_render_target_()
	{};
	~Triangle() {};

	//初期化処理
	void init(ID3D12Device* device);
	//描画処理
	void draw(RenderContext* rc);

private:
	//ルートシグネチャの作成
	void initRootSignature(ID3D12Device* device);
	//シェーダーのロード
	void loadShader();
	//パイプラインステートオブジェクトの作成
	void initPipelineStateObject(ID3D12Device* device);
	//頂点バッファの作成
	void initVertexBuffer(ID3D12Device* device);
	//インデックスバッファの作成
	void initIndexBuffer(ID3D12Device* device);
	//オフスクリーンレンダーターゲットの作成
	void initOffScreenRenderTarget(ID3D12Device* device);
	//深度ステンシルの作成
	void initDepthStencil(ID3D12Device* device);

public:

	/// <summary>
	/// 描画モードの設定
	/// </summary>
	/// <param name="mode"></param>
	void setRenderMode(RenderMode mode) {
		this->render_mode_ = mode;
	};
public:
	//頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	//頂点データの設定
	void setVertices(Vertex vertices[3]);

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
		return this->vertices_[index];
	};

private:

	std::shared_ptr<RootSignature> root_signature_;                                   //ルートシグニチャ
	std::shared_ptr<Shader> vertex_shader_;                                           //頂点シェーダー
	std::shared_ptr<Shader> pixel_shader_;                                            //ピクセルシェーダー
	std::shared_ptr<PipelineStateObject> pso_[(int)RenderMode::Num]; //パイプラインステートオブジェクト
	std::shared_ptr<VertexBuffer> vertex_buffer_;                                     //頂点バッファ
	std::shared_ptr<IndexBuffer> index_buffer_;                                       //インデックスバッファ
	std::shared_ptr<DepthStencil> depth_stencil_;                                     //深度ステンシル
	Vertex vertices_[3];										                        //頂点データ
	RenderMode render_mode_;														    //描画モード
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;                   //オフスクリーンレンダーターゲット
	ID3D12Device* device_;
};
