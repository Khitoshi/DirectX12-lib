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
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->pipelineStateObject.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertexBuffer.get());
    //�C���f�b�N�X�o�b�t�@��ݒ�B
    rc->setIndexBuffer(this->indexBuffer.get());
    //�h���[�R�[��
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
    //���_���C�A�E�g���`����B
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

    //������
    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->init(vertexBufferConf);

    //�R�s�[
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
