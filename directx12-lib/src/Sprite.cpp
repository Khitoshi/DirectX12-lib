#include "Sprite.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="conf">テクスチャの設定</param>
void Sprite::init(const SpriteConf conf)
{
    initRootSignature(conf);
    loadShader();
    initPipelineStateObject(conf);
    initVertexBuffer(conf);
    initIndexBuffer(conf);
}

void Sprite::draw(RenderContext* rc)
{
    //ルートシグネチャを設定。
    rc->setRootSignature(this->rootSignature.get());
    //パイプラインステートを設定。
    rc->setPipelineState(this->pipelineStateObject.get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertexBuffer.get());
    //インデックスバッファを設定。
    rc->setIndexBuffer(this->indexBuffer.get());
    //ドローコール
    rc->drawIndexed(numIndices);
}

void Sprite::initRootSignature(SpriteConf conf)
{
    rootSignature = std::make_shared<RootSignature>();
    RootSignatureConf rootSignatureConf = {};
    rootSignatureConf.device = conf.device;
    rootSignature->init(rootSignatureConf);
}

void Sprite::loadShader()
{
    //頂点シェーダーのロード
    vertexShader = std::make_shared<Shader>();
    ShaderConf vsConf = {};
    vsConf.filePath = "./src/shaders/SpriteVS.hlsl";
    vsConf.entryFuncName = "VSMain";
    vertexShader->LoadVS(vsConf);

    //ピクセルシェーダーのロード
    pixelShader = std::make_shared<Shader>();
    ShaderConf psConf = {};
    psConf.filePath = "./src/shaders/SpritePS.hlsl";
    psConf.entryFuncName = "PSMain";
    pixelShader->LoadPS(psConf);
}

void Sprite::initPipelineStateObject(SpriteConf conf)
{
    // インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
    };

    //パイプラインステートオブジェクトの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
    desc.pRootSignature = this->rootSignature->getRootSignature();
    desc.VS = CD3DX12_SHADER_BYTECODE(vertexShader->getShaderBlob());
    desc.PS = CD3DX12_SHADER_BYTECODE(pixelShader->getShaderBlob());
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

    // ソリッドモード初期化
    PipelineStateObjectConf psoConf = {};
    psoConf.device = conf.device;
    psoConf.desc = desc;
    pipelineStateObject = std::make_shared<PipelineStateObject>();
    pipelineStateObject->init(psoConf);
}

void Sprite::initVertexBuffer(SpriteConf conf)
{
    //頂点データ
    
    vertices[0] = {
        { -0.25f, -0.25f, 0.5f }, {0,1}, // 左下
    };
    vertices[1] = {
        { -0.25f,  0.25f, 0.5f }, {0,0}, // 左上
    };
    vertices[2] = {
        {  0.25f,  0.25f, 0.5f }, {1,1}, // 右上
    };
    vertices[3] = {
        {  0.25f, -0.25f, 0.5f }, {1,0}, // 右下
    };
    

    //頂点バッファの設定
    VertexBufferConf vertexBufferConf = {};
    vertexBufferConf.device = conf.device;
    vertexBufferConf.size = sizeof(vertices);
    vertexBufferConf.stride = sizeof(Vertex);

    //初期化
    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->init(vertexBufferConf);

    //コピー
    vertexBuffer->copy(vertices);
}

void Sprite::initIndexBuffer(SpriteConf conf)
{
    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3
    };
    //unsigned short* indices = new unsigned short[6] { 0, 1, 2, 0, 2, 3 };

    //インデックスバッファの設定
    numIndices = sizeof(indices) / sizeof(unsigned short);
    IndexBufferConf indexBufferConf = {};
    indexBufferConf.device = conf.device;
    indexBufferConf.size = sizeof(indices) * numIndices;// 4 bytes * 要素数 indices
    indexBufferConf.stride = sizeof(unsigned short);
    indexBufferConf.count = numIndices;

    //初期化
    indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->init(indexBufferConf);
    //コピー
    indexBuffer->copy(static_cast<uint16_t*>(indices));
}
