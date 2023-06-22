#include "Triangle.h"

void Triangle::init(TriangleConf conf)
{

    loadShader();
    initPipelineStateObject(conf);
    initVertexBuffer(conf);
    initIndexBuffer(conf);

}

void Triangle::draw(RenderContext* rc)
{
    //パイプラインステートを設定。
    rc->setPipelineState(this->pipelineStateObject.get());
    //プリミティブのトポロジーを設定。
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //頂点バッファを設定。
    rc->setVertexBuffer(this->vertexBuffer.get());
    //インデックスバッファを設定。
    rc->setIndexBuffer(this->indexBuffer.get());
    //ドローコール
    rc->drawIndexed(3);
}

void Triangle::loadShader()
{
    vertexShader = std::make_shared<Shader>();
    //vertexShader->LoadVS("./shaders/TriangleVS.hlsl", "VSmain");
    vertexShader->LoadVS("./src/shaders/TriangleVS.hlsl", "VSmain");

    pixelShader = std::make_shared<Shader>();
    pixelShader->LoadPS("./src/shaders/TrianglePS.hlsl", "PSmain");
}

void Triangle::initPipelineStateObject(TriangleConf conf)
{
    //頂点レイアウトを定義する。
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
    desc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    desc.pRootSignature = conf.rootSignature->getRootSignature();
    desc.VS = CD3DX12_SHADER_BYTECODE(this->vertexShader->getShaderBlob());
    desc.PS = CD3DX12_SHADER_BYTECODE(this->pixelShader->getShaderBlob());
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    desc.DepthStencilState.DepthEnable = FALSE;
    desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    desc.DepthStencilState.StencilEnable = FALSE;
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pipelineStateObject = std::make_shared<PipelineStateObject>();
    pipelineStateObject->init(conf.device, desc);
}

void Triangle::initVertexBuffer(TriangleConf conf)
{

   vertices[0] = {
            {-0.5f, -0.5f, 0.0f},
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f }
    };
    vertices[1] = {
            { 0.0f, 0.5f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.5f, 1.0f }
    };
    vertices[2] = {
            { 0.5f, -0.5f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            {1.0f, 0.0f}
    };


    VertexBufferConf vertexBufferConf;
    vertexBufferConf.device = conf.device;
    vertexBufferConf.size = sizeof(vertices);
    vertexBufferConf.stride = sizeof(vertices[0]);

    //初期化
    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->init(vertexBufferConf);

    //コピー
    vertexBuffer->copy(vertices);
}

void Triangle::initIndexBuffer(TriangleConf conf)
{
    unsigned short indices[] = {
        0,1,2
    };

    IndexBufferConf indexBufferConf = {};
    indexBufferConf.device= conf.device;
    indexBufferConf.size = sizeof(indices);
    indexBufferConf.stride = sizeof(indices[0]);

    indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->init(indexBufferConf);
    indexBuffer->copy(indices);

}
