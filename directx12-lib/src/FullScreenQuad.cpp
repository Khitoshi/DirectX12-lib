#include "FullScreenQuad.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void FullScreenQuad::init(ID3D12Device* device)
{
    createShader(device);
    createVertexBuffer(device);
    createRootSignature(device);
    createPipelineState(device);
    createSRVHeap(device);
    this->num_indices_ = 4;
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
/// <param name="osrt">オフスクリーンレンダーターゲット</param>
void FullScreenQuad::draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* crt)
{
    //ルートシグネチャを設定
    rc->setRootSignature(this->root_signature_.get());
    //パイプラインステートを設定
    rc->setPipelineState(this->pso_.get());
    //プリミティブのトポロジーを設定
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //頂点バッファを設定
    rc->setVertexBuffer(this->vertex_buffer_.get());
    //ディスクリプタ作成
    this->createSRV(device, crt);
    //SRVヒープを設定
    rc->setDescriptorHeap(this->srv_heap_.Get());
    //GPUハンドルをcommandlistに設定
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = this->srv_heap_->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpuHandle);

    //ドローコール
    rc->drawInstanced(this->num_indices_);
}

/// <summary>
/// Basicシェーダーのペア生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void FullScreenQuad::createShader(ID3D12Device* device)
{
    {//頂点シェーダー
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        conf.entryFuncName = "VSMain";
        conf.filePath = "./src/shaders/FullScreenQuadVS.hlsl";
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {//ピクセルシェーダー
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        conf.entryFuncName = "PSMain";
        conf.filePath = "./src/shaders/FullScreenQuadPS.hlsl";
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// 頂点バッファの作成
/// </summary>
/// <param name="device">GPUデバイス</param>
void FullScreenQuad::createVertexBuffer(ID3D12Device* device)
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
    vertex_buffer_ = std::make_shared<VertexBuffer>();
    vertex_buffer_->init(conf);
    vertex_buffer_->copy(vertices);
}

/// <summary>
/// ルートシグニチャの作成
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void FullScreenQuad::createRootSignature(ID3D12Device* device)
{
    //ルートシグニチャの設定(レジスタを使用しないので空にする)
    RootSignatureConf conf = {};
    conf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    conf.samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    conf.textureAddressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    conf.textureAddressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    conf.textureAddressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    conf.numSampler = 1;
    conf.maxSrvDescriptor = 1;
    conf.offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// パイプラインステートオブジェクト作成
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void FullScreenQuad::createPipelineState(ID3D12Device* device)
{
    // インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
    };

    //パイプラインステートオブジェクトの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
    desc.pRootSignature = this->root_signature_->getRootSignature();
    desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
    desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    desc.SampleMask = UINT_MAX;
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    desc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.NodeMask = 1;
    desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    PipelineStateObject::PipelineStateObjectConf conf;
    conf.desc = desc;

    this->pso_ = PSOCacheManager::getInstance().getPSO(device, conf);
}

void FullScreenQuad::createSRVHeap(ID3D12Device* device)
{
    //SRV用のディスクリプタヒープを作成
    D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
    srv_heap_desc.NumDescriptors = 1;
    srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&srv_heap_desc, IID_PPV_ARGS(&this->srv_heap_)))) {
        throw new std::exception("failed to create SRV heap");
    }
}

void FullScreenQuad::createSRV(ID3D12Device* device, CompositeRenderTarget* osrt)
{
    //SRVの作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv_desc.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(osrt->getResource(), &srv_desc, this->srv_heap_->GetCPUDescriptorHandleForHeapStart());
}

