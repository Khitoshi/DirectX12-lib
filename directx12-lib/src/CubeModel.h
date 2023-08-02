#pragma once
#include "d3dx12.h"
#include <memory>
#include <DirectXMath.h>

class RootSignature;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class DepthStencil;
class OffScreenRenderTarget;

class RenderContext;

class CubeModel
{
public:
    //定数バッファへのデータ転送用構造体
    struct CubeModelConf
    {
        DirectX::XMFLOAT4X4  model;
        DirectX::XMFLOAT4X4  view;
        DirectX::XMFLOAT4X4  projection;
    };

public:
    CubeModel(const CubeModelConf c) :
        conf_(c),
        root_signature_(),
        vertex_shader_(),
        pixel_shader_(),
        pso_(),
        vertex_buffer_(),
        index_buffer_(),
        constant_buffer_(),
        vertices_(),
        num_indices_(0),
        off_screen_render_target_()
    {};
    ~CubeModel() {};

    //初期化処理
    void init(ID3D12Device* device);
    //更新処理
    void update();
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
    //定数バッファの作成
    void initConstantBuffer(ID3D12Device* device);
    //オフスクリーンレンダーターゲットの作成
    void initOffScreenRenderTarget(ID3D12Device* device);
    //深度ステンシルの作成
    void initDepthStencil(ID3D12Device* device);

public:
    void setConf(const CubeModelConf c) { conf_ = c; }
private:
    //頂点データ
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
    };

    CubeModelConf conf_;
    std::shared_ptr<RootSignature> root_signature_;                     //ルートシグニチャ
    std::shared_ptr<Shader> vertex_shader_;                             //頂点シェーダー
    std::shared_ptr<Shader> pixel_shader_;                              //ピクセルシェーダー
    std::shared_ptr<PipelineStateObject> pso_;                          //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertex_buffer_;                       //頂点バッファ
    std::shared_ptr<IndexBuffer> index_buffer_;                         //インデックスバッファ
    std::shared_ptr<ConstantBuffer> constant_buffer_;                   //定数バッファ
    std::shared_ptr<DepthStencil> depth_stencil_;                       //深度ステンシル
    Vertex vertices_[8];										        //頂点データ
    UINT num_indices_;                                                  //インデックス数
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;   //オフスクリーンレンダーターゲット

};