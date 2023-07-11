#include "FullScreenQuad.h"

void FullScreenQuad::init(ID3D12Device* device)
{
    createShader(device);
    createVertexBuffer(device);
    //createIndexBuffer(device);
    createRootSignature(device);
    createPipelineState(device);
}

void FullScreenQuad::draw(RenderContext* rc)
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
    //rc->setIndexBuffer(this->indexBuffer.get());
    //ドローコール
    rc->drawInstanced(4);
}

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
/// <param name="device"></param>
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

void FullScreenQuad::createIndexBuffer(ID3D12Device* device)
{
    //インデックスバッファの作成
    uint16_t indices[] = {
        0,1,2,
        2,1,3
    };
    //インデックスバッファの設定
    this->numIndices = sizeof(indices) / sizeof(unsigned short);
    IndexBufferConf conf = {};
    conf.device = device;
    conf.size = sizeof(indices) * numIndices;
    conf.stride = sizeof(uint16_t);
    conf.count = numIndices;
    indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->init(conf);
    indexBuffer->copy(indices);
}

/// <summary>
/// ルートシグニチャの作成
/// </summary>
/// <param name="device"></param>
void FullScreenQuad::createRootSignature(ID3D12Device* device)
{
    //ルートシグニチャの設定(レジスタを使用しないので空にする)
    RootSignatureConf conf = {};
    conf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignature = std::make_shared<RootSignature>();
    rootSignature->init(device, conf);
}

void FullScreenQuad::createPipelineState(ID3D12Device* device)
{
    PipelineStateObject::PipelineStateObjectConf conf;
    // インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
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
