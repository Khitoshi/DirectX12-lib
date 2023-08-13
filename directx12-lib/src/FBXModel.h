#include "d3dx12.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>

class RootSignature;
class DescriptorHeap;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class DepthStencil;
class Texture;
class OffScreenRenderTarget;
class RenderContext;
class FBXModelData;

//fbxモデルクラス
class FBXModel
{
public:
    //定数バッファへのデータ転送用構造体
    struct ModelConf
    {
        DirectX::XMFLOAT4X4  model;
        DirectX::XMFLOAT4X4  view;
        DirectX::XMFLOAT4X4  projection;
    };
public:
    FBXModel(const ModelConf& c) :
        conf_(c),
        model_data_(),
        root_signature_(),
        srv_cbv_uav_descriptor_heap_(),
        vertex_shader_(),
        pixel_shader_(),
        pso_(),
        vertex_buffer_(),
        index_buffer_(),
        constant_buffer_(),
        num_indices_(0),
        depth_stencil_(),
        off_screen_render_target_(),
        device_(0)
    {};
    ~FBXModel() {};

    void init(ID3D12Device* device, const char* model_file_path);
    void update();
    void draw(RenderContext* rc);
private:
    void loadModel(ID3D12Device* device, const char* model_file_path);
    void loadShader();
    void initRootSignature(ID3D12Device* device);
    void initDescriptorHeap(ID3D12Device* device);
    void initPipelineStateObject(ID3D12Device* device);
    void initVertexBuffer(ID3D12Device* device);
    void initIndexBuffer(ID3D12Device* device);
    void initConstantBuffer(ID3D12Device* device);
    void initDepthStencil(ID3D12Device* device);
    void initOffScreenRenderTarget(ID3D12Device* device);

public:
    void setConf(const ModelConf& c) { conf_ = c; }
    void setModel(const char* model_file_path);
private:
    ModelConf conf_;
    std::shared_ptr<FBXModelData> model_data_;
    std::shared_ptr<RootSignature> root_signature_;                     //ルートシグニチャ
    std::shared_ptr<DescriptorHeap> srv_cbv_uav_descriptor_heap_;       //ディスクリプタヒープ
    std::shared_ptr<Shader> vertex_shader_;                             //頂点シェーダー
    std::shared_ptr<Shader> pixel_shader_;                              //ピクセルシェーダー
    std::shared_ptr<PipelineStateObject> pso_;                          //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertex_buffer_;                       //頂点バッファ
    std::shared_ptr<IndexBuffer> index_buffer_;                         //インデックスバッファ
    UINT num_indices_;                                                  //インデックス数
    std::shared_ptr<ConstantBuffer> constant_buffer_;                   //定数バッファ
    std::shared_ptr<DepthStencil> depth_stencil_;                       //深度ステンシル
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;   //オフスクリーンレンダーターゲット

    ID3D12Device* device_;
};
