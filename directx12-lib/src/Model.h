#include "d3dx12.h"

#ifdef _DEBUG
#pragma comment(lib,"assimp-vc142-mtd.lib")
#else
#pragma comment(lib,"assimp-vc142-md.lib")
#endif // _DEBUG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelData.h"
#include <vector>

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

class Model
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
    Model(const ModelConf& c) :
        conf_(c),
        root_signature_(),
        vertex_shader_(),
        pixel_shader_(),
        pso_(),
        vertex_buffer_(),
        index_buffer_(),
        faces_(),
        constant_buffer_(),
        vertices_(),
        num_indices_(0),
        texture_(),
        depth_stencil_(),
        off_screen_render_target_()
    {};
    ~Model() {};

    void init(ID3D12Device* device, const char* model_file_path);
    void update();
    void draw(RenderContext* rc);
private:
    void parseNode(const aiScene* scene, const aiMesh* mesh);

    void loadShader();
    void initRootSignature(ID3D12Device* device);
    void initDescriptorHeap(ID3D12Device* device);
    void initPipelineStateObject(ID3D12Device* device);
    void initVertexBuffer(ID3D12Device* device);
    void initIndexBuffer(ID3D12Device* device);
    void initConstantBuffer(ID3D12Device* device);
    void initTexture(ID3D12Device* device, const char* texture_file_path);
    void initDepthStencil(ID3D12Device* device);
    void initOffScreenRenderTarget(ID3D12Device* device);

public:
    void setConf(const ModelConf& c) { conf_ = c; }
private:
    ModelConf conf_;
    std::vector<ModelData::MeshVertex> vertices_;
    std::shared_ptr<RootSignature> root_signature_;                     //ルートシグニチャ
    std::shared_ptr<DescriptorHeap> srv_cbv_uav_descriptor_heap_;                  //ディスクリプタヒープ
    std::shared_ptr<Shader> vertex_shader_;                             //頂点シェーダー
    std::shared_ptr<Shader> pixel_shader_;                              //ピクセルシェーダー
    std::shared_ptr<PipelineStateObject> pso_;                          //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertex_buffer_;                       //頂点バッファ
    std::shared_ptr<IndexBuffer> index_buffer_;                         //インデックスバッファ
    std::vector<ModelData::MeshFace> faces_;
    UINT num_indices_;
    std::shared_ptr<ConstantBuffer> constant_buffer_;                   //定数バッファ
    std::shared_ptr<DepthStencil> depth_stencil_;                       //深度ステンシル
    std::shared_ptr<Texture> texture_;
    std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;   //オフスクリーンレンダーターゲット
};
