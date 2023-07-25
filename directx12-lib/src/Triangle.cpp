#include "Triangle.h"
#include "OffScreenRenderTargetCacheManager.h"
/// <summary>
/// 三角形に必要なリソースの初期化
/// </summary>
/// <param name="conf">三角形の描画用設定</param>
void Triangle::init(TriangleConf conf)
{
    initRootSignature(conf);
    loadShader();
    initPipelineStateObject(conf);
    initVertexBuffer(conf);
    initIndexBuffer(conf);
    initOffScreenRenderTarget(conf);
}

/// <summary>
/// 三角形描画
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
void Triangle::draw(RenderContext* rc)
{
    //オフスクリーンレンダーターゲットで書き込みできる状態にする
    auto renderTarget = offScreenRenderTarget->getRTVHeap();
    auto resource = offScreenRenderTarget->getResource();
    auto depthStencil = OffScreenRenderTargetCacheManager::getInstance().getDepthStencilViewHandle();
    auto viewport = OffScreenRenderTargetCacheManager::getInstance().getViewport();

    //ビューポートとシザリング矩形の設定
    rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depthStencil, resource);



    //ルートシグネチャを設定。
    rc->setRootSignature(this->rootSignature.get());
    //パイプラインステートを設定。
    rc->setPipelineState(this->pipelineStateObject[(int)renderMode].get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertexBuffer.get());
    //インデックスバッファを設定。
    rc->setIndexBuffer(this->indexBuffer.get());
    //ドローコール
    rc->drawIndexed(3);

    //オフスクリーンレンダーターゲットの書き込みを終了する。
    offScreenRenderTarget->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(offScreenRenderTarget.get());
}

/// <summary>
/// ルートシグニチャの初期化
/// </summary>
/// <param name="conf">三角形の描画用設定</param>
void Triangle::initRootSignature(TriangleConf conf)
{
    //ルートシグネチャの設定(レジスタを使用しないので空にする)
    RootSignatureConf rootSignatureConf = {};
    rootSignatureConf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignature = std::make_shared<RootSignature>();
    rootSignature->init(conf.device, rootSignatureConf);
}

/// <summary>
/// シェーダーのロード(コンパイル)
/// </summary>
void Triangle::loadShader()
{
    {
        //頂点シェーダーのロード
        vertexShader = std::make_shared<Shader>();
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/TriangleVS.hlsl";
        conf.entryFuncName = "VSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        vertexShader->load(conf);
    }

    {
        //ピクセルシェーダーのロード
        pixelShader = std::make_shared<Shader>();
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/TrianglePS.hlsl";
        conf.entryFuncName = "PSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        pixelShader->load(conf);
    }
}

/// <summary>
/// psoの初期化
/// </summary>
/// <param name="conf">三角形の描画用設定</param>
void Triangle::initPipelineStateObject(TriangleConf conf)
{
    // インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
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
    PipelineStateObject::PipelineStateObjectConf psoConf = {};
    psoConf.desc = desc;
    pipelineStateObject[(int)RenderMode::Solid] = std::make_shared<PipelineStateObject>(psoConf);
    pipelineStateObject[(int)RenderMode::Solid]->init(conf.device);

    // ワイヤーフレームモード初期化
    D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    desc.RasterizerState = rasterizerDesc;
    psoConf.desc = desc;
    pipelineStateObject[(int)RenderMode::WireFrame] = std::make_shared<PipelineStateObject>(psoConf);
    pipelineStateObject[(int)RenderMode::WireFrame]->init(conf.device);

}

/// <summary>
/// 頂点バッファの初期化
/// </summary>
/// <param name="conf">三角形の描画用設定</param>
void Triangle::initVertexBuffer(TriangleConf conf)
{
    //頂点データ
    vertices[0] = {
        {-0.5f, -0.5f,  0.0f},
        {1.0f,  0.0f,   0.0f,  1.0f},
    };
    vertices[1] = {
        { 0.0f, 0.5f, 0.0f },
        { 0.0f, 1.0f, 0.0f ,1.0f},
    };
    vertices[2] = {
        { 0.5f, -0.5f,  0.0f },
        { 0.0f, 0.0f,   1.0f ,  1.0f},
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
/// <param name="conf">三角形の描画用設定</param>
void Triangle::initIndexBuffer(TriangleConf conf)
{
    unsigned short indices[] = {
        0,1,2
    };

    //インデックスバッファの設定
    const int numIndices = sizeof(indices) / sizeof(unsigned short);
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

void Triangle::initOffScreenRenderTarget(TriangleConf conf)
{
    OffScreenRenderTarget::OffScreenRenderTargetConf osrtConf = {};
    osrtConf = OffScreenRenderTargetCacheManager::getInstance().getConf();
    offScreenRenderTarget = std::make_shared<OffScreenRenderTarget>(osrtConf);
    offScreenRenderTarget->init(conf.device);
}
