#include "CompositeRenderTarget.h"

//#include "RootSignature.h"
#include "DescriptorHeapCache.h"

#include "OffScreenRenderTargetCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "PSOCacheManager.h"
#include "ShaderCacheManager.h"
#include "Shader.h"
#include <stdexcept>

/// <summary>
/// 初期化
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void CompositeRenderTarget::init(ID3D12Device* device)
{
    createResource(device);
    createSRVHeap(device);
    createShaderResourceView(device);
    createRTVHeap(device);
    createRenderTargetView(device);

    initRootSignature(device);
    initShader(device);
    initVertexBuffer(device);
    initPipelineStateObject(device);
}

/// <summary>
/// offscreenに描画をする為の初期処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
void CompositeRenderTarget::beginRender(RenderContext* rc, D3D12_VIEWPORT viewport, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle)
{
    rc->simpleStart(this->rtv_heap_->GetCPUDescriptorHandleForHeapStart(), depthStencilViewHandle, this->resource_.Get());
}

void CompositeRenderTarget::render(RenderContext* rc, ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle)
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
        //ディスクリプタヒープを設定
        DescriptorHeapCache::DescriptorHeapCacheConf dhcConf = {};
        dhcConf.resource = rt->getResource();
        dhcConf.slot = slot;
        this->descriptor_heap_cache_->getOrCreate(device, dhcConf, this->srv_heap_.Get(), srv_desc_);
        slot++;
    }
    rc->setDescriptorHeap(this->srv_heap_.Get());
    //GPUハンドルをcommandlistに設定
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = this->srv_heap_->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpuHandle);

    //ドローコール
    rc->drawInstanced(4);
    OffScreenRenderTargetCacheManager::getInstance().clearRenderTargetList();
}

/// <summary>
/// offscreenに描画をする為の終了処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
void CompositeRenderTarget::endRender(RenderContext* rc)
{
    //レンダーターゲットのRESOURCE_BARRIER設定
    rc->TransitionTemporaryRenderTargetAwait(this->resource_.Get());
}

/// <summary>
/// リソースを作成する
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void CompositeRenderTarget::createResource(ID3D12Device* device)
{
    D3D12_RESOURCE_DESC desc = {};
    desc = this->conf_.resourceDesc;
    D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, conf_.clearColor);

    if (FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(this->resource_.ReleaseAndGetAddressOf())
    ))) {
        throw std::runtime_error("failed to create offscreen render target");
    }
}

/// <summary>
/// シェーダーリソースビュー用のディスクリプタヒープを作成する
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void CompositeRenderTarget::createSRVHeap(ID3D12Device* device)
{
    //SRVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf_.descriptorHeapDesc;
    desc.NumDescriptors = 2;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->srv_heap_.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// シェーダーリソースビューを作成する
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void CompositeRenderTarget::createShaderResourceView(ID3D12Device* device)
{
    ////シェーダーリソースビューを作成
    srv_desc_ = {};
    srv_desc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc_.Texture2D.MipLevels = 1;
    srv_desc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

/// <summary>
/// レンダーターゲットビュー用のディスクリプタヒープを作成する
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void CompositeRenderTarget::createRTVHeap(ID3D12Device* device)
{
    //RTVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf_.descriptorHeapDesc;
    desc.NumDescriptors = 1;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->rtv_heap_.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// レンダーターゲットビューを作成する
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void CompositeRenderTarget::createRenderTargetView(ID3D12Device* device)
{
    //レンダーターゲットビューを作成
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateRenderTargetView(this->resource_.Get(), &rtvDesc, this->rtv_heap_->GetCPUDescriptorHandleForHeapStart());
}

/// <summary>
/// ルートシグニチャを作成する
/// </summary>
/// <param name="device"></param>
void CompositeRenderTarget::initRootSignature(ID3D12Device* device)
{
    RootSignatureConf rootSignatureConf = {};

    rootSignatureConf.samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.textureAddressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.numSampler = 1;
    rootSignatureConf.maxSrvDescriptor = 2;
    rootSignatureConf.offsetInDescriptorsFromTableStartSRV = 0;
    rootSignatureConf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

/// <summary>
/// シェーダーを初期化する
/// </summary>
/// <param name="device"></param>
void CompositeRenderTarget::initShader(ID3D12Device* device)
{
    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/MultiPathCompositingVS.hlsl";
        conf.entryFuncName = "VSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }

    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/MultiPathCompositingPS.hlsl";
        conf.entryFuncName = "PSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// パイプラインステートオブジェクトの初期化
/// </summary>
/// <param name="device"></param>
void CompositeRenderTarget::initPipelineStateObject(ID3D12Device* device)
{
    //インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
    };

    //深度ステンシルステート
    D3D12_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = FALSE; // 深度テストを無効にする
    dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // 深度値の書き込みを無効にする
    dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // 深度比較関数を常に真にする
    // ステンシル設定も無効にするなら以下も設定
    dsDesc.StencilEnable = FALSE;
    dsDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    dsDesc.FrontFace = defaultStencilOp;
    dsDesc.BackFace = defaultStencilOp;


    //パイプラインステートオブジェクトの設定
    PipelineStateObject::PipelineStateObjectConf PSOConf = {};
    PSOConf.desc.pRootSignature = this->root_signature_->getRootSignature();
    PSOConf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
    PSOConf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
    PSOConf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PSOConf.desc.SampleMask = UINT_MAX;
    PSOConf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PSOConf.desc.DepthStencilState = dsDesc;
    PSOConf.desc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    PSOConf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    PSOConf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSOConf.desc.NumRenderTargets = 1;
    PSOConf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    PSOConf.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    PSOConf.desc.SampleDesc.Count = 1;
    PSOConf.desc.SampleDesc.Quality = 0;
    PSOConf.desc.NodeMask = 1;
    PSOConf.desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    this->pso_ = PSOCacheManager::getInstance().getPSO(device, PSOConf);
}

void CompositeRenderTarget::initVertexBuffer(ID3D12Device* device)
{
    //頂点バッファの作成
    Vertex vertices[] = {
        {{ -1.0f, -1.0f, 0.0f }, {0,1}}, // 左下
        {{ -1.0f,  1.0f, 0.0f }, {0,0}}, // 左上
        {{  1.0f, -1.0f, 0.0f }, {1,1}}, // 右下
        {{  1.0f,  1.0f, 0.0f }, {1,0}}, // 右上
    };

    VertexBufferConf conf = {};
    conf.device = device;
    conf.size = sizeof(vertices);
    conf.stride = sizeof(Vertex);
    this->vertex_buffer_ = std::make_shared<VertexBuffer>();
    this->vertex_buffer_->init(conf);
    this->vertex_buffer_->copy(vertices);
}

void CompositeRenderTarget::initDescriptorHeapCache()
{
    this->descriptor_heap_cache_ = std::make_unique<DescriptorHeapCache>();
}
