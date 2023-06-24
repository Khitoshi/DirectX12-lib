#include "Triangle.h"

void Triangle::init(TriangleConf conf)
{

    loadShader();
    initPipelineStateObject(conf);
    initVertexBuffer(conf);
    //initIndexBuffer(conf);

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
    //rc->setIndexBuffer(this->indexBuffer.get());
    //ドローコール
    rc->drawIndexed(3);
}

void Triangle::loadShader()
{
    vertexShader = std::make_shared<Shader>();
    vertexShader->LoadVS("./src/shaders/TriangleVS.hlsl", "VSMain");

    pixelShader = std::make_shared<Shader>();
    pixelShader->LoadPS("./src/shaders/TrianglePS.hlsl", "PSMain");
}

void Triangle::initPipelineStateObject(TriangleConf conf)
{
    // インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
    desc.pRootSignature = conf.rootSignature->getRootSignature();
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

    pipelineStateObject = std::make_shared<PipelineStateObject>();
    pipelineStateObject->init(conf.device, desc);
}

void Triangle::initVertexBuffer(TriangleConf conf)
{

    vertices[0] = {
             {  0.0f, 0.25f, 0.5f }, { 1.0f, 0.0f,0.0f,1.0f },
    };
    vertices[1] = {
            { 0.25f,-0.25f, 0.5f}, { 0.0f, 1.0f,0.0f,1.0f},
    };
    vertices[2] = {
            {-0.25f,-0.25f, 0.5f }, { 0.0f, 0.0f,1.0f,1.0f},
    };


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

void Triangle::initIndexBuffer(TriangleConf conf)
{
    unsigned short indices[] = {
        0,1,2
    };

    IndexBufferConf indexBufferConf = {};
    indexBufferConf.device = conf.device;
    indexBufferConf.size = sizeof(indices);
    indexBufferConf.stride = sizeof(indices[0]);

    indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->init(indexBufferConf);
    indexBuffer->copy(indices);
}
