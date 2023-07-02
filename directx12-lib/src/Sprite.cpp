#include "Sprite.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="conf">テクスチャの設定</param>
void Sprite::init(const SpriteConf conf)
{
    initRootSignature(conf);
    initShader();
    initPipelineStateObject(conf);
    initVertexBuffer(conf);
    initIndexBuffer(conf);
    initTexture(conf);
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc"></param>
void Sprite::draw(RenderContext* rc)
{
    //ルートシグネチャを設定。
    rc->setRootSignature(this->rootSignature.get());
    //パイプラインステートを設定。
    rc->setPipelineState(this->defaultPipelineStateObject.get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertexBuffer.get());
    //インデックスバッファを設定。
    rc->setIndexBuffer(this->indexBuffer.get());

    //テクスチャを設定。
    rc->setTexture(this->texture.get());

    //ドローコール
    rc->drawIndexed(numIndices);
}

/// <summary>
/// ルートシグニチャの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initRootSignature(SpriteConf conf)
{
    RootSignatureConf rootSignatureConf = {};

    rootSignatureConf.samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.textureAdressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAdressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAdressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.numSampler = 1;
    rootSignatureConf.maxSrvDescriptor = 1;
    rootSignatureConf.offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    rootSignatureConf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    this->psoParameter.rootSignatureConf = rootSignatureConf;
    //rootSignature = std::make_shared<RootSignature>();
    //rootSignature->init(conf.device, rootSignatureConf);
}

/// <summary>
/// シェーダーのロード(コンパイル)
/// </summary>
void Sprite::initShader()
{
    this->psoParameter.vertexShaderConf.filePath = "./src/shaders/SpriteVS.hlsl";
    this->psoParameter.vertexShaderConf.entryFuncName = "VSMain";
    this->psoParameter.vertexShaderConf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;

    this->psoParameter.pixelShaderConf.filePath = "./src/shaders/SpritePS.hlsl";
    this->psoParameter.pixelShaderConf.entryFuncName = "PSMain";
    this->psoParameter.pixelShaderConf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
}

/// <summary>
/// psoの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initPipelineStateObject(SpriteConf conf)
{
    // インプットレイアウト
    PSOCacheManager::PipelineStateSettings settings = {};

    settings.pipelineStateDesc.blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    settings.pipelineStateDesc.sampleMask = UINT_MAX;
    settings.pipelineStateDesc.rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    settings.pipelineStateDesc.depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    D3D12_INPUT_ELEMENT_DESC inputElementDescPos = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA };
    D3D12_INPUT_ELEMENT_DESC inputElementDescTex = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA };
    settings.pipelineStateDesc.inputLayout.push_back(inputElementDescPos);
    settings.pipelineStateDesc.inputLayout.push_back(inputElementDescTex);

    settings.pipelineStateDesc.iBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    settings.pipelineStateDesc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    settings.pipelineStateDesc.numRenderTargets = 1;
    settings.pipelineStateDesc.rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    settings.pipelineStateDesc.dsvFormat = DXGI_FORMAT_D32_FLOAT;

    settings.pipelineStateDesc.sampleDesc.Count = 1;
    settings.pipelineStateDesc.sampleDesc.Quality = 0;

    settings.pipelineStateDesc.nodeMask = 1;
    settings.pipelineStateDesc.flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    this->psoParameter.pipelineStateSettings = settings;

    this->defaultPipelineStateObject = PSOCacheManager::getInstance().getPSO(conf.device, this->psoParameter);
}

/// <summary>
/// 頂点バッファの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initVertexBuffer(SpriteConf conf)
{
    //頂点データ
    vertices[0] = {
        //{ -0.25f, -0.25f, 0.5f }, {0,1}, // 左下
        { -0.75f, -0.75f, 0.0f }, {0,1}, // 左下
    };
    vertices[1] = {
        //{ -0.25f,  0.25f, 0.5f }, {0,0}, // 左上
        { -0.75f,  0.75f, 0.0f }, {0,0}, // 左上
    };
    vertices[2] = {
        //{  0.25f,  0.25f, 0.5f }, {1,0}, // 右上
        {  0.75f,  0.75f, 0.0f }, {1,0}, // 右上
    };
    vertices[3] = {
        //{  0.25f, -0.25f, 0.5f }, {1,1}, // 右下
        {  0.75f, -0.75f, 0.0f }, {1,1}, // 右下
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

/// <summary>
/// インデックスバッファの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initIndexBuffer(SpriteConf conf)
{
    //インデックスデータ
    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3
    };

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

/// <summary>
/// テクスチャの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initTexture(SpriteConf conf)
{
    //テクスチャの初期化
    Texture::TextureConf textureConf = {};
    textureConf.device = conf.device;
    textureConf.filePath = conf.filePath;
    texture = std::make_shared<Texture>(textureConf);
    texture->Load();
}
