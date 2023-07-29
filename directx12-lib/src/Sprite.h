#pragma once
#include "d3dx12.h"

#include <memory>
#include <DirectXMath.h>
#include <stdexcept>
#include <map>

class RootSignature;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class Texture;
class Camera;
class RenderContext;
class Rotation;
class RotationEffect;
class OffScreenRenderTarget;
class Shader;
class DepthStencil;

using namespace Microsoft::WRL;

/// <summary>
/// 画像描画用のクラス
/// </summary>
class Sprite
{
public:
    struct SpriteConf
    {
        const char* file_path;
        Camera* camera;
    };

public:
    Sprite(SpriteConf c) :
        conf_(c),
        root_signature_(nullptr),
        pixel_shader_(nullptr),
        vertex_shader_(nullptr),
        pso_(nullptr),
        vertex_buffer_(nullptr),
        index_buffer_(nullptr),
        num_indices_(0),
        vertices_(),
        texture_(nullptr),
        off_screen_render_target_(nullptr)
    {};
    ~Sprite() {};

    //初期化処理
    void init(ID3D12Device* device);
    //描画処理
    void draw(RenderContext* rc);

private:

    //ルートシグネチャの作成
    void initRootSignature(ID3D12Device* device);
    //シェーダーのロード
    void initShader();
    //パイプラインステートオブジェクトの作成
    void initPipelineStateObject(ID3D12Device* device);

    //頂点バッファの作成
    void initVertexBuffer(ID3D12Device* device);
    //インデックスバッファの作成
    void initIndexBuffer(ID3D12Device* device);
    //テクスチャの作成
    void initTexture(ID3D12Device* device);
    //オフスクリーンレンダーターゲットの作成
    void initOffScreenRenderTarget(ID3D12Device* device);
    //深度ステンシルの作成
    void initDepthStencil(ID3D12Device* device);
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
    void setVertices(Vertex vertices[4]);

public://取得系
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
        return vertices_[index];
    };

private:
    enum class SpriteOptions : uint8_t {
        NONE = 1 << 0, // 1
        ROTATED = 1 << 1, // 2
        // 他のオプション...

        NUM,
    };

    SpriteConf conf_;
    std::shared_ptr<RootSignature> root_signature_;                     //ルートシグニチャ
    std::shared_ptr<Shader> pixel_shader_;                              //ピクセルシェーダー
    std::shared_ptr<Shader> vertex_shader_;                             //頂点シェーダー
    std::shared_ptr<PipelineStateObject> pso_;                          //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertex_buffer_;                       //頂点バッファ
    std::shared_ptr<IndexBuffer> index_buffer_;                         //インデックスバッファ
    Vertex vertices_[4];										        //頂点データ
    unsigned int num_indices_;                                          //インデックス数
    std::shared_ptr<Texture> texture_;									//テクスチャ
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;   //オフスクリーンレンダーターゲット
    std::shared_ptr< DepthStencil> depth_stencil_;                      //深度ステンシル
};
