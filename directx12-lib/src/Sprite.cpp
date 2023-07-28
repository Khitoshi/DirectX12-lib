#include "Sprite.h"
//#include "ShaderCacheManager.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "OffScreenRenderTargetFactory.h"
#include "IndexBufferFactory.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="conf">テクスチャの設定</param>
void Sprite::init()
{
    initRootSignature();
    initShader();
    initPipelineStateObject();
    initVertexBuffer();
    initIndexBuffer();
    initTexture();
    /*
    this->rotationEffect = std::make_shared<RotationEffect>();
    this->rotationEffect->init(conf.device, conf.camera);
    */


    initOffScreenRenderTarget();
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc"></param>
void Sprite::draw(RenderContext* rc)
{
    {
        //オフスクリーンレンダーターゲットで書き込みできる状態にする
        auto renderTarget = offScreenRenderTarget->getRTVHeap();
        auto resource = offScreenRenderTarget->getResource();
        auto depthStencil = OffScreenRenderTargetCacheManager::getInstance().getDepthStencilViewHandle();
        auto viewport = OffScreenRenderTargetCacheManager::getInstance().getViewport();

        //ビューポートとシザリング矩形の設定
        rc->transitionOffScreenRenderTargetBegin(resource);
        //rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depthStencil, resource);
        rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depthStencil);
    }


    //ルートシグネチャを設定。
    rc->setRootSignature(this->rootSignature.get());
    //パイプラインステートを設定。
    rc->setPipelineState(this->defaultPipelineStateObject.get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertexBuffer.get());
    //インデックスバッファを設定。
    rc->setIndexBuffer(this->indexBuffer.get());
    //テクスチャを設定。
    rc->setTexture(this->texture.get());

    //ドローコール
    rc->drawIndexed(this->numIndices);

    /*
    this->rotationEffect->update(rc, conf.camera, this->vertexBuffer.get(), this->indexBuffer.get(), this->numIndices);
    rc->setIndexBuffer(this->indexBuffer.get());
    rc->setVertexBuffer(this->vertexBuffer.get());
    */

    //オフスクリーンレンダーターゲットの書き込みを終了する。
    offScreenRenderTarget->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(offScreenRenderTarget.get());
}

/// <summary>
/// ルートシグニチャの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initRootSignature()
{
    RootSignatureConf rootSignatureConf = {};

    rootSignatureConf.samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.textureAddressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.numSampler = 1;
    rootSignatureConf.maxSrvDescriptor = 1;
    rootSignatureConf.offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    rootSignatureConf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    this->rootSignature = RootSignatureCacheManager::getInstance().getOrCreate(conf.device, rootSignatureConf);
}

/// <summary>
/// シェーダーのロード(コンパイル)
/// </summary>
void Sprite::initShader()
{
    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/SpriteVS.hlsl";
        conf.entryFuncName = "VSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        defaultShaderPair.vertexShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }

    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/SpritePS.hlsl";
        conf.entryFuncName = "PSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        defaultShaderPair.pixelShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// psoの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initPipelineStateObject()
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
    };

    // インプットレイアウト
    PipelineStateObject::PipelineStateObjectConf PSOConf = {};
    PSOConf.desc.pRootSignature = this->rootSignature->getRootSignature();
    PSOConf.desc.VS = CD3DX12_SHADER_BYTECODE(defaultShaderPair.vertexShader->getShaderBlob());
    PSOConf.desc.PS = CD3DX12_SHADER_BYTECODE(defaultShaderPair.pixelShader->getShaderBlob());
    PSOConf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PSOConf.desc.SampleMask = UINT_MAX;
    PSOConf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PSOConf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

    this->defaultPipelineStateObject = PSOCacheManager::getInstance().getPSO(conf.device, PSOConf);
}

/// <summary>
/// 頂点バッファの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initVertexBuffer()
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
void Sprite::initIndexBuffer()
{
    //インデックスデータ
    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3
    };

    //インデックスバッファの設定
    numIndices = sizeof(indices) / sizeof(unsigned short);
    IndexBuffer::IndexBufferConf indexBufferConf = {};
    indexBufferConf.size = sizeof(indices) * numIndices;// 4 bytes * 要素数 indices
    indexBufferConf.stride = sizeof(unsigned short);
    indexBufferConf.count = numIndices;

    //初期化
    indexBuffer = IndexBufferFactory::create(indexBufferConf, conf.device);
    //コピー
    indexBuffer->copy(static_cast<uint16_t*>(indices));
}

/// <summary>
/// テクスチャの初期化
/// </summary>
/// <param name="conf"></param>
void Sprite::initTexture()
{
    //テクスチャの初期化
    Texture::TextureConf textureConf = {};
    textureConf.device = conf.device;
    textureConf.filePath = conf.filePath;
    texture = std::make_shared<Texture>(textureConf);
    texture->Load();
}



void Sprite::initOffScreenRenderTarget()
{
    OffScreenRenderTarget::OffScreenRenderTargetConf osrtConf = {};
    osrtConf = OffScreenRenderTargetCacheManager::getInstance().getConf();
    offScreenRenderTarget = OffScreenRenderTargetFactory::create(osrtConf, conf.device);
}