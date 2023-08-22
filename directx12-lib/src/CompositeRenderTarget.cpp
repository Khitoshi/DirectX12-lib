#include "CompositeRenderTarget.h"
#include "DescriptorCache.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "ShaderCacheManager.h"
#include "CommonGraphicsConfig.h"
#include "VertexBufferFactory.h"
#include "RenderContext.h"
#include <stdexcept>

/// <summary>
/// 初期化
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void CompositeRenderTarget::init(ID3D12Device* device)
{
    createResource(device);
    createSRVHeap(device);
    createSRVDesc(device);
    createRTVHeap(device);
    createRTV(device);

    initRootSignature(device);
    initShader(device);
    initVertexBuffer(device);
    initPipelineStateObject(device);
    initDescriptorHeapCache();
}

/// <summary>
/// 描画初期処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
/// <param name="depthStencilViewHandle">深度ステンシルビューのハンドル</param>
void CompositeRenderTarget::beginRender(RenderContext* rc, D3D12_CPU_DESCRIPTOR_HANDLE depthStencil_view_handle)
{
    //リソースの状態を遷移
    rc->transitionOffScreenRenderTargetBegin(this->resource_.Get());
    //描画開始処理
    rc->simpleStart(this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), depthStencil_view_handle);
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::render(RenderContext* rc, ID3D12Device* device)
{
    //ルートシグネチャを設定。
    rc->setRootSignature(this->root_signature_.get());
    //パイプラインステートを設定。
    rc->setPipelineState(this->pso_.get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertex_buffer_.get());

    // SRVHeapの始点を取得
    UINT slot = 0;
    for (auto& rt : OffScreenRenderTargetCacheManager::getInstance().getRenderTargetList())
    {
        //ディスクリプタの生成
        DescriptorCache::DescriptorCacheConf dhcConf = {};
        dhcConf.resource = rt->getResource();
        dhcConf.slot = slot;
        this->descriptor_cache_->getOrCreate(device, dhcConf, this->cbv_srv_uav_descriptor_heap_->getDescriptorHeap(), srv_desc_);
        slot++;
    }
    //ディスクリプタヒープを設定
    rc->setDescriptorHeap(this->cbv_srv_uav_descriptor_heap_.get());

    //GPUハンドルをcommandlistに設定
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = this->cbv_srv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpu_handle);

    gpu_handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 1;
    rc->setGraphicsRootDescriptorTable(1, gpu_handle);

    //ドローコール
    rc->drawInstanced(4);
}

/// <summary>
/// offscreenに描画をする為の終了処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
void CompositeRenderTarget::endRender(RenderContext* rc)
{
    //オフスクリーンリストをクリア
    OffScreenRenderTargetCacheManager::getInstance().clearRenderTargetList();
    //レンダーターゲットのRESOURCE_BARRIER設定
    rc->transitionOffScreenRenderTargetEnd(this->resource_.Get());
}

/// <summary>
/// リソースを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::createResource(ID3D12Device* device)
{
    D3D12_RESOURCE_DESC desc = {};
    desc = this->conf_.resource_desc;
    D3D12_HEAP_PROPERTIES heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_CLEAR_VALUE clear_value = {};
    clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, backGroundColor);
    if (FAILED(device->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clear_value,
        IID_PPV_ARGS(this->resource_.ReleaseAndGetAddressOf())
    ))) {
        throw std::runtime_error("failed to create offscreen render target");
    }
}

/// <summary>
/// シェーダーリソースビュー用のディスクリプタヒープを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::createSRVHeap(ID3D12Device* device)
{
    this->cbv_srv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
}

/// <summary>
/// シェーダーリソースビューを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::createSRVDesc(ID3D12Device* device)
{
    ////シェーダーリソースビューの設定
    srv_desc_ = {};
    srv_desc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc_.Texture2D.MipLevels = 1;
    srv_desc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

/// <summary>
/// レンダーターゲットビュー用のディスクリプタヒープを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::createRTVHeap(ID3D12Device* device)
{
    this->rtv_descriptor_heap_ = DescriptorHeapFactory::create(device, this->conf_.descriptor_heap_desc.Type, 1);
}

/// <summary>
/// レンダーターゲットビューを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::createRTV(ID3D12Device* device)
{
    D3D12_RENDER_TARGET_VIEW_DESC desc = {};
    desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateRenderTargetView(this->resource_.Get(), &desc, this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

/// <summary>
/// ルートシグニチャを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::initRootSignature(ID3D12Device* device)
{
    RootSignature::RootSignatureConf rs_conf = {};
    rs_conf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rs_conf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rs_conf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rs_conf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rs_conf.num_sampler = 1;
    rs_conf.num_srv = 2;
    rs_conf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rs_conf);
}

/// <summary>
/// シェーダーを初期化する
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::initShader(ID3D12Device* device)
{
    //TODO :動的にシェーダーを作成するようにする
    //作成したシェーダーはキャッシュする
    {//頂点シェーダーロード
        Shader::ShaderConf conf = {};
        conf.file_path = "./src/shaders/MultiPathCompositingVS.hlsl";
        conf.entry_func_name = "VSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }

    {//ピクセルシェーダーロード
        Shader::ShaderConf conf = {};
        conf.file_path = "./src/shaders/MultiPathCompositingPS.hlsl";
        conf.entry_func_name = "PSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// パイプラインステートオブジェクトの初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::initPipelineStateObject(ID3D12Device* device)
{
    //インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC input_rlement_desc[] = {
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

    //パイプラインステートオブジェクトの設定
    PipelineStateObject::PipelineStateObjectConf pso_conf = {};
    pso_conf.desc.pRootSignature = this->root_signature_->getRootSignature();
    pso_conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
    pso_conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
    pso_conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso_conf.desc.SampleMask = UINT_MAX;
    pso_conf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_conf.desc.DepthStencilState = ds_desc;
    pso_conf.desc.InputLayout = { input_rlement_desc, _countof(input_rlement_desc) };
    pso_conf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pso_conf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_conf.desc.NumRenderTargets = 1;
    pso_conf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso_conf.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pso_conf.desc.SampleDesc.Count = 1;
    pso_conf.desc.SampleDesc.Quality = 0;
    pso_conf.desc.NodeMask = 1;
    pso_conf.desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    this->pso_ = PSOCacheManager::getInstance().getOrCreate(device, pso_conf);
}

/// <summary>
/// 頂点バッファを初期化処理
/// </summary>
/// <param name="device">GPUデバイス</param>
void CompositeRenderTarget::initVertexBuffer(ID3D12Device* device)
{
    Vertex vertices[] = {
        {{ -1.0f, -1.0f, 0.0f }, {0,1}}, // 左下
        {{ -1.0f,  1.0f, 0.0f }, {0,0}}, // 左上
        {{  1.0f, -1.0f, 0.0f }, {1,1}}, // 右下
        {{  1.0f,  1.0f, 0.0f }, {1,0}}, // 右上
    };

    VertexBuffer::VertexBufferConf vb_conf = {};
    vb_conf.size = sizeof(vertices);
    vb_conf.stride = sizeof(Vertex);

    this->vertex_buffer_ = VertexBufferFactory::create(vb_conf, device);
    this->vertex_buffer_->copy(vertices);
}

/// <summary>
/// ディスクリプタヒープのキャッシュを初期化
/// </summary>
void CompositeRenderTarget::initDescriptorHeapCache()
{
    this->descriptor_cache_ = std::make_unique<DescriptorCache>();
}

ID3D12DescriptorHeap* CompositeRenderTarget::getRTVHeap() const
{
    return rtv_descriptor_heap_->getDescriptorHeap();
}
