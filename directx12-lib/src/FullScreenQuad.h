#pragma once
#include "d3dx12.h"
#include <memory>
#include <DirectXMath.h>

class VertexBuffer;
class IndexBuffer;
class RootSignature;
class DescriptorHeap;
class PipelineStateObject;
class Shader;
class RenderContext;
class Texture;
class CompositeRenderTarget;


using namespace Microsoft::WRL;

/// <summary>
/// オフスクリーンレンダーターゲットをフルスクリーンに描画するクラス
/// </summary>
class FullScreenQuad
{
    friend class FullScreenQuadFactory;
public:
    //頂点情報構造体
    struct Vertex
    {
        DirectX::XMFLOAT3 position;	//頂点座標
        DirectX::XMFLOAT2 uv;		//テクスチャ座標
    };

private:
    FullScreenQuad() :
        vertex_buffer_(),
        index_buffer_(),
        root_signature_(),
        pso_(),
        pixel_shader_(),
        vertex_shader_(),
        texture_(),
        num_indices_(0),
        cbv_srv_descriptor_heap_()
    {};

public:
    ~FullScreenQuad() {};
    //描画処理
    void draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* osrt);

private:
    //初期化処理
    void init(ID3D12Device* device);
    //シェーダーのペア
    void createShader(ID3D12Device* device);
    //頂点バッファの作成
    void createVertexBuffer(ID3D12Device* device);
    //ルートシグネチャの作成
    void createRootSignature(ID3D12Device* device);
    //パイプラインステートの作成
    void createPipelineState(ID3D12Device* device);
    //シェーダーリソースビューヒープの作成
    void createSRVHeap(ID3D12Device* device);
    //シェーダーリソースビューの作成
    void createSRV(ID3D12Device* device, CompositeRenderTarget* osrt);

private:
    std::shared_ptr<VertexBuffer> vertex_buffer_;	//頂点バッファ
    std::shared_ptr<IndexBuffer> index_buffer_;		//インデックスバッファ
    std::shared_ptr<RootSignature> root_signature_; //ルートシグネチャ
    std::shared_ptr<PipelineStateObject> pso_;		//パイプラインステートオブジェクト
    std::shared_ptr<Shader> pixel_shader_;			//ピクセルシェーダー
    std::shared_ptr<Shader> vertex_shader_;			//バーテックスシェーダー
    std::shared_ptr<Texture> texture_;				//テクスチャ
    UINT num_indices_;								//インデックスの数
    std::shared_ptr<DescriptorHeap> cbv_srv_descriptor_heap_;  //シェーダーリソースビューディスクリプタヒープ
};
