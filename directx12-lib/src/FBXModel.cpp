#include "FBXModel.h"
//#include "FBXModelData.h"
//#include "FBXModelDataFactory.h"
#include "Mesh.h"
#include "AssimpLoader.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "IndexBufferFactory.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "DepthStencilCacheManager.h"
#include "ShaderCacheManager.h"
#include "ConstantBufferFactory.h"
#include "TextureCacheManager.h"
#include "RenderContext.h"
#include "CommonGraphicsConfig.h"
#include <stdexcept>
#include <math.h>

void FBXModel::init(ID3D12Device* device, const char* model_file_path)
{
    this->device_ = device;
    this->initRootSignature(device);
    this->initDescriptorHeap(device);
    this->loadShader();
    this->initPipelineStateObject(device);
    this->loadModel(device, model_file_path);
    this->initConstantBuffer(device);
    this->initTexture(device);
    this->initVertexBuffer(device);
    this->initIndexBuffer(device);
    this->initOffScreenRenderTarget(device);
    this->initDepthStencil(device);
}

void FBXModel::update()
{
    this->constant_buffer_->copy(&this->conf_);
}

void FBXModel::draw(RenderContext* rc)
{
    {
        //オフスクリーンレンダーターゲットで書き込みできる状態にする
        auto renderTarget = this->off_screen_render_target_->getRTVHeap();
        auto resource = this->off_screen_render_target_->getResource();
        auto depth_stencil = this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        //ビューポートとシザリング矩形の設定
        rc->transitionOffScreenRenderTargetBegin(resource);
        rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depth_stencil);
    }

    for (size_t i = 0; i < this->meshes_.size(); i++)
    {
        //ルートシグネチャを設定。
        rc->setRootSignature(this->root_signature_.get());
        //パイプラインステートを設定。
        rc->setPipelineState(this->pso_.get());
        //プリミティブのトポロジーを設定。
        rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //頂点バッファを設定。
        rc->setVertexBuffer(this->vertex_buffers_[i].get());
        //インデックスバッファを設定
        rc->setIndexBuffer(this->index_buffers_[i].get());

        //ディスクリプタヒープを設定
        rc->setDescriptorHeap(this->srv_cbv_uav_descriptor_heap_.get());

        auto handle = this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
        rc->setGraphicsRootDescriptorTable(0, handle);

        handle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (1 + i);
        rc->setGraphicsRootDescriptorTable(1, handle);

        //handle = this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
        //handle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (2 + i);
        rc->setGraphicsRootDescriptorTable(2, this->meshes_[i].material.handle_gPU);
        //rc->setGraphicsRootDescriptorTable(2, handle);

        //ドローコール
        rc->drawIndexed(this->num_indices_[i]);
    }

    //オフスクリーンレンダーターゲットの書き込みを終了する。
    this->off_screen_render_target_->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

/// <summary>
/// ルートシグネチャの初期化
/// </summary>
/// <param name="device"></param>
void FBXModel::initRootSignature(ID3D12Device* device)
{
    RootSignature::RootSignatureConf rootSignatureConf = {};
    rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.num_cbv = 2;
    rootSignatureConf.num_srv = 1;
    rootSignatureConf.num_sampler = 1;
    rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
    rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

/// <summary>
/// ディスクリプタヒープの初期化
/// </summary>
/// <param name="device"></param>
void FBXModel::initDescriptorHeap(ID3D12Device* device)
{
    this->srv_cbv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 16);
}

/// <summary>
/// モデルの読み込み
/// </summary>
/// <param name="device">GPUデバイス</param>
/// <param name="model_file_path">モデルのファイルパス</param>
void FBXModel::loadModel(ID3D12Device* device, const char* model_file_path)
{
    //model_data_ = FBXModelDataFactory::create(device, this->srv_cbv_uav_descriptor_heap_.get(), model_file_path);
    meshes_ = AssimpLoader::Load(model_file_path, false, false);
}

/// <summary>
/// シェーダーの読み込み
/// </summary>
void FBXModel::loadShader()
{
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "VSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
        conf.file_path = "./src/shaders/3dModelVS.hlsl";
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "PSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
        conf.file_path = "./src/shaders/3dModelPS.hlsl";
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// パイプラインステートオブジェクトの初期化
/// </summary>
/// <param name="device"></param>
void FBXModel::initPipelineStateObject(ID3D12Device* device)
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
        {"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
        {"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
        {"TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
        {"COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
    };

    //深度ステンシルステート
    D3D12_DEPTH_STENCIL_DESC ds_desc = {};
    //ds_desc.DepthEnable = FALSE; // 深度テストを無効にする
    ds_desc.DepthEnable = TRUE; // 深度テストを無効にする
    //ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;   // 深度値の書き込みを無効にする
    ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    //ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;       // 深度比較関数を常に真にする
    ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // 新しいフラグメントが現在の深度よりも小さい場合に描画
    ds_desc.StencilEnable = FALSE;
    ds_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    ds_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    ds_desc.FrontFace = defaultStencilOp;
    ds_desc.BackFace = defaultStencilOp;

    //ラスタライザーステート
    D3D12_RASTERIZER_DESC rasterizer_desc = {};
    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID; // ソリッド表示
    //rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE; // カリングオフ
    //rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK; // 背面カリングオン
    //rasterizer_desc.CullMode = D3D12_CULL_MODE_FRONT;
    rasterizer_desc.FrontCounterClockwise = FALSE; // 通常の順序
    rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // インプットレイアウト
    PipelineStateObject::PipelineStateObjectConf conf = {};
    conf.desc.pRootSignature = this->root_signature_->getRootSignature();
    conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
    conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
    conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    conf.desc.SampleMask = UINT_MAX;
    conf.desc.RasterizerState = rasterizer_desc;
    conf.desc.DepthStencilState = ds_desc;
    //conf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    conf.desc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    conf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    conf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    conf.desc.NumRenderTargets = 1;
    conf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    conf.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    conf.desc.SampleDesc.Count = 1;
    conf.desc.SampleDesc.Quality = 0;
    conf.desc.NodeMask = 1;
    conf.desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    this->pso_ = PSOCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// 頂点バッファの初期化
/// </summary>
/// <param name="device"></param>
void FBXModel::initVertexBuffer(ID3D12Device* device)
{
    //初期化
    const size_t mesh_size = this->meshes_.size();
    this->vertex_buffers_.resize(mesh_size);
    for (size_t i = 0; i < mesh_size; i++) {
        //頂点バッファの設定
        auto& vertices = this->meshes_[i].vertices;

        VertexBuffer::VertexBufferConf conf = {};
        conf.size = sizeof(Vertex) * vertices.size();
        conf.stride = sizeof(Vertex);
        auto vb = VertexBufferFactory::create(conf, device);
        vb->copy(vertices.data());

        this->vertex_buffers_[i] = vb;
    }
}

/// <summary>
/// インデックスバッファの初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void FBXModel::initIndexBuffer(ID3D12Device* device)
{
    const size_t mesh_size = this->meshes_.size();
    this->index_buffers_.resize(mesh_size);
    this->num_indices_.resize(mesh_size);
    for (size_t i = 0; i < mesh_size; i++)
    {
        auto& indices = this->meshes_[i].indices;
        //インデックスバッファの設定
        this->num_indices_[i] = static_cast<UINT>(indices.size());
        IndexBuffer::IndexBufferConf indexBufferConf = {};
        indexBufferConf.size = sizeof(indices.data()) * this->num_indices_[i];// 4 bytes * 要素数 indices
        indexBufferConf.stride = sizeof(uint32_t);
        indexBufferConf.count = this->num_indices_[i];

        //初期化
        auto ib = IndexBufferFactory::create(indexBufferConf, device);
        //コピー
        ib->copy(indices.data());

        this->index_buffers_[i] = ib;
    }
}

/// <summary>
/// 定数バッファの初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void FBXModel::initConstantBuffer(ID3D12Device* device)
{
    {
        ConstantBuffer::ConstantBufferConf conf = {};
        conf.size = sizeof(ModelConf);
        conf.descriptor_heap = this->srv_cbv_uav_descriptor_heap_.get();
        conf.slot = 0;
        this->constant_buffer_ = ConstantBufferFactory::create(device, conf);

        this->constant_buffer_->copy(&this->conf_);
    }


    {
        struct MaterialConf
        {
            DirectX::XMFLOAT4 diffuse_color; // 拡散色
            // 他のマテリアルプロパティもここに追加できます
        };

        mesh_color_constant_buffers_.resize(this->meshes_.size());
        for (size_t i = 0; i < this->meshes_.size(); i++) {
            MaterialConf materialConf = {};
            materialConf.diffuse_color = this->meshes_[i].material.diffuse_color; // ここでXMFLOAT4に適切な値を設定する

            ConstantBuffer::ConstantBufferConf conf = {};
            conf.size = sizeof(MaterialConf);
            conf.descriptor_heap = this->srv_cbv_uav_descriptor_heap_.get();
            conf.slot = 1 + i;
            mesh_color_constant_buffers_[i] = ConstantBufferFactory::create(device, conf);
            mesh_color_constant_buffers_[i]->copy(&materialConf);
        }

    }

}

void FBXModel::initTexture(ID3D12Device* device)
{
    for (size_t i = 0; i < meshes_.size(); i++) {
        auto tex = TextureCacheManager::getInstance().getOrCreate(device, meshes_[i].material.diffuse_map_name.c_str());
        tex->CreateShaderResourceView(device, this->srv_cbv_uav_descriptor_heap_.get(), this->meshes_.size() + 1 + i);
        auto handle = this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
        handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (this->meshes_.size() + 1 + i);
        this->meshes_[i].material.handle_gPU = handle;
    }
}


/// <summary>
/// オフスクリーンレンダーターゲットの初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void FBXModel::initOffScreenRenderTarget(ID3D12Device* device)
{
    OffScreenRenderTarget::OffScreenRenderTargetConf osrtConf = {};

    {//ディスクリプタヒープの設定
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = 1;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0;

        osrtConf.descriptor_heap_desc = desc;
    }
    {//リソースデスクの設定
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = windowWidth;
        desc.Height = windowHeight;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        osrtConf.resource_desc = desc;
    }

    this->off_screen_render_target_ = OffScreenRenderTargetFactory::create(osrtConf, device);
}

/// <summary>
/// 深度ステンシルの初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void FBXModel::initDepthStencil(ID3D12Device* device)
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.frame_buffer_count = 1;
    ds_conf.width = windowWidth;
    ds_conf.height = windowHeight;
    this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

/// <summary>
/// モデルの設定
/// </summary>
/// <param name="model_file_path">モデルのファイルパス</param>
void FBXModel::setModel(const char* model_file_path)
{
    //this->model_data_ = FBXModelDataFactory::create(this->device_, this->srv_cbv_uav_descriptor_heap_.get(), model_file_path);
    //this->initIndexBuffer(this->device_);
    //this->initVertexBuffer(device_);
}
