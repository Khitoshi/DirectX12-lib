#include "CubeModel.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "IndexBufferFactory.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "DepthStencilCacheManager.h"
#include "ShaderCacheManager.h"
#include "ConstantBufferFactory.h"
#include "RenderContext.h"
#include "CommonGraphicsConfig.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device"></param>
void CubeModel::init(ID3D12Device* device)
{
    this->loadShader();
    this->initRootSignature(device);
    this->initDescriptorHeap(device);
    this->initPipelineStateObject(device);
    this->initVertexBuffer(device);
    this->initIndexBuffer(device);
    this->initConstantBuffer(device);
    this->initOffScreenRenderTarget(device);
    this->initDepthStencil(device);
}

/// <summary>
/// 更新処理
/// </summary>
void CubeModel::update()
{
    this->constant_buffer_->copy(&this->conf_);
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc"></param>
void CubeModel::draw(RenderContext* rc)
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

    //ルートシグネチャを設定。
    rc->setRootSignature(this->root_signature_.get());
    //パイプラインステートを設定。
    rc->setPipelineState(this->pso_.get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertex_buffer_.get());
    //インデックスバッファを設定
    rc->setIndexBuffer(this->index_buffer_.get());

    //定数バッファを設定
    //rc->setConstantBufferView(this->constant_buffer_.get());
    std::vector<DescriptorHeap*>ds;
    ds.push_back(this->descriptor_heap_.get());
    rc->setDescriptorHeaps(ds);
    rc->setGraphicsRootDescriptorTable(0, this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    //ドローコール
    rc->drawIndexed(this->num_indices_);

    //オフスクリーンレンダーターゲットの書き込みを終了する。
    this->off_screen_render_target_->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

/// <summary>
/// ルートシグネチャの初期化
/// </summary>
/// <param name="device"></param>
void CubeModel::initRootSignature(ID3D12Device* device)
{
    RootSignature::RootSignatureConf rootSignatureConf = {};

    rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.num_cbv = 1;
    rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

/// <summary>
/// ディスクリプタヒープの作成
/// </summary>
/// <param name="device"></param>
void CubeModel::initDescriptorHeap(ID3D12Device* device)
{
    this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

/// <summary>
/// シェーダーの読み込み
/// </summary>
void CubeModel::loadShader()
{
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "VSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
        conf.file_path = "./src/shaders/CubeVS.hlsl";
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "PSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
        conf.file_path = "./src/shaders/CubePS.hlsl";
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// パイプラインステートオブジェクトの初期化
/// </summary>
/// <param name="device"></param>
void CubeModel::initPipelineStateObject(ID3D12Device* device)
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
    };

    //深度ステンシルステート
    D3D12_DEPTH_STENCIL_DESC ds_desc = {};
    ds_desc.DepthEnable = FALSE; // 深度テストを無効にする
    ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;   // 深度値の書き込みを無効にする
    ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;       // 深度比較関数を常に真にする
    ds_desc.StencilEnable = FALSE;
    ds_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    ds_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    ds_desc.FrontFace = defaultStencilOp;
    ds_desc.BackFace = defaultStencilOp;

    D3D12_RASTERIZER_DESC rasterizer_desc = {};
    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID; // ソリッド表示
    rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE; // カリングオフ
    //rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK; // 背面カリングオン
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
void CubeModel::initVertexBuffer(ID3D12Device* device)
{
    //頂点データ
    this->vertices_[0] = { {-0.75f, -0.75f, 0.0f }, { 0, 1 } };    // 左下前
    this->vertices_[1] = { {-0.75f, 0.75f,  0.0f }, { 0, 0 } };     // 左上前
    this->vertices_[2] = { {0.75f,  0.75f,  0.0f }, { 1, 0 } };      // 右上前
    this->vertices_[3] = { {0.75f,  -0.75f, 0.0f }, { 1, 1 } };     // 右下前
    this->vertices_[4] = { {-0.75f, -0.75f, -1.0f },{ 0, 1 } };   // 左下奥
    this->vertices_[5] = { {-0.75f, 0.75f,  -1.0f },{ 0, 0 } };    // 左上奥
    this->vertices_[6] = { {0.75f,  0.75f,  -1.0f },{ 1, 0 } };     // 右上奥
    this->vertices_[7] = { {0.75f,  -0.75f, -1.0f },{ 1, 1 } };    // 右下奥

    //頂点バッファの設定
    VertexBuffer::VertexBufferConf conf = {};
    conf.size = sizeof(this->vertices_);
    conf.stride = sizeof(Vertex);

    //初期化
    this->vertex_buffer_ = VertexBufferFactory::create(conf, device);
    //コピー
    this->vertex_buffer_->copy(this->vertices_);
}

/// <summary>
/// インデックスバッファの初期化
/// </summary>
/// <param name="device"></param>
void CubeModel::initIndexBuffer(ID3D12Device* device)
{
    //インデックスデータ
    unsigned short indices[] = {
    0, 1, 2, 2, 3, 0, // 前面
    4, 5, 6, 6, 7, 4, // 背面
    0, 1, 5, 5, 4, 0, // 左側面
    2, 3, 7, 7, 6, 2, // 右側面
    1, 2, 6, 6, 5, 1, // 上面
    0, 3, 7, 7, 4, 0  // 下面
    };

    //インデックスバッファの設定
    this->num_indices_ = sizeof(indices) / sizeof(unsigned short);
    IndexBuffer::IndexBufferConf indexBufferConf = {};
    indexBufferConf.size = sizeof(indices) * this->num_indices_;// 4 bytes * 要素数 indices
    indexBufferConf.stride = sizeof(unsigned short);
    indexBufferConf.count = this->num_indices_;

    //初期化
    this->index_buffer_ = IndexBufferFactory::create(indexBufferConf, device);
    //コピー
    this->index_buffer_->copy(static_cast<uint16_t*>(indices));
}

/// <summary>
/// 定数バッファの初期化
/// </summary>
/// <param name="device"></param>
void CubeModel::initConstantBuffer(ID3D12Device* device)
{
    ConstantBuffer::ConstantBufferConf conf = {};
    conf.size = sizeof(CubeModelConf);
    conf.descriptor_heap = this->descriptor_heap_.get();
    conf.slot = 0;
    this->constant_buffer_ = ConstantBufferFactory::create(device, conf);
    constant_buffer_->copy(&this->conf_);
}

/// <summary>
/// オフスクリーンレンダーターゲットの初期化
/// </summary>
/// <param name="device"></param>
void CubeModel::initOffScreenRenderTarget(ID3D12Device* device)
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
/// <param name="device"></param>
void CubeModel::initDepthStencil(ID3D12Device* device)
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.frame_buffer_count = 1;
    ds_conf.width = windowWidth;
    ds_conf.height = windowHeight;
    this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}