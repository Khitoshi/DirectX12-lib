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
    this->numIndices = 4;
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
/// <param name="osrt">オフスクリーンレンダーターゲット</param>
void FullScreenQuad::draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* crt)
{
    //ルートシグネチャを設定。
    rc->setRootSignature(this->rootSignature.get());
    //パイプラインステートを設定。
    rc->setPipelineState(this->pso.get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertexBuffer.get());
    //インデックスバッファを設定。
    //rc->setTexture(crt->getSRVHeap(), 1);
    this->createSRV(device, crt);
    rc->setDescriptorHeap(this->SRVHeap.Get());
    //GPUハンドルをcommandlistに設定
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = this->SRVHeap->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpuHandle);

    //ドローコール
    rc->drawInstanced(this->numIndices);
}

/// <summary>
/// Basicシェーダーのペア生成
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void FullScreenQuad::createShader(ID3D12Device* device)
{
    {
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        conf.entryFuncName = "VSMain";
        conf.filePath = "./src/shaders/FullScreenQuadVS.hlsl";
        basicShaderPair.vertexShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        conf.entryFuncName = "PSMain";
        conf.filePath = "./src/shaders/FullScreenQuadPS.hlsl";
        basicShaderPair.pixelShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// 頂点バッファの作成
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
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
    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->init(conf);
    vertexBuffer->copy(vertices);
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

    this->rootSignature = RootSignatureCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// パイプラインステートオブジェクト作成
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void FullScreenQuad::createPipelineState(ID3D12Device* device)
{
    PipelineStateObject::PipelineStateObjectConf conf;
    // インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
    };

    //パイプラインステートオブジェクトの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
    desc.pRootSignature = this->rootSignature->getRootSignature();
    desc.VS = CD3DX12_SHADER_BYTECODE(this->basicShaderPair.vertexShader->getShaderBlob());
    desc.PS = CD3DX12_SHADER_BYTECODE(this->basicShaderPair.pixelShader->getShaderBlob());
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
    conf.desc = desc;

    this->pso = PSOCacheManager::getInstance().getPSO(device, conf);
}

void FullScreenQuad::createSRVHeap(ID3D12Device* device)
{
    //SRV用のディスクリプタヒープを作成
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&this->SRVHeap)))) {
        throw new std::exception("failed to create SRV heap");
    }
}

void FullScreenQuad::createSRV(ID3D12Device* device, CompositeRenderTarget* osrt)
{
    //SRVの作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(osrt->getResource(), &srvDesc, this->SRVHeap->GetCPUDescriptorHandleForHeapStart());
}

