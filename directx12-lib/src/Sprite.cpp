#include "Sprite.h"

/// <summary>
/// ����������
/// </summary>
/// <param name="conf">�e�N�X�`���̐ݒ�</param>
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
    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->rootSignature.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->pipelineStateObject.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertexBuffer.get());
    //�C���f�b�N�X�o�b�t�@��ݒ�B
    rc->setIndexBuffer(this->indexBuffer.get());
    //�h���[�R�[��
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
    //���_�V�F�[�_�[�̃��[�h
    vertexShader = std::make_shared<Shader>();
    ShaderConf vsConf = {};
    vsConf.filePath = "./src/shaders/SpriteVS.hlsl";
    vsConf.entryFuncName = "VSMain";
    vertexShader->LoadVS(vsConf);

    //�s�N�Z���V�F�[�_�[�̃��[�h
    pixelShader = std::make_shared<Shader>();
    ShaderConf psConf = {};
    psConf.filePath = "./src/shaders/SpritePS.hlsl";
    psConf.entryFuncName = "PSMain";
    pixelShader->LoadPS(psConf);
}

void Sprite::initPipelineStateObject(SpriteConf conf)
{
    // �C���v�b�g���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
    };

    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐ݒ�
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

    // �\���b�h���[�h������
    PipelineStateObjectConf psoConf = {};
    psoConf.device = conf.device;
    psoConf.desc = desc;
    pipelineStateObject = std::make_shared<PipelineStateObject>();
    pipelineStateObject->init(psoConf);
}

void Sprite::initVertexBuffer(SpriteConf conf)
{
    //���_�f�[�^
    
    vertices[0] = {
        { -0.25f, -0.25f, 0.5f }, {0,1}, // ����
    };
    vertices[1] = {
        { -0.25f,  0.25f, 0.5f }, {0,0}, // ����
    };
    vertices[2] = {
        {  0.25f,  0.25f, 0.5f }, {1,1}, // �E��
    };
    vertices[3] = {
        {  0.25f, -0.25f, 0.5f }, {1,0}, // �E��
    };
    

    //���_�o�b�t�@�̐ݒ�
    VertexBufferConf vertexBufferConf = {};
    vertexBufferConf.device = conf.device;
    vertexBufferConf.size = sizeof(vertices);
    vertexBufferConf.stride = sizeof(Vertex);

    //������
    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->init(vertexBufferConf);

    //�R�s�[
    vertexBuffer->copy(vertices);
}

void Sprite::initIndexBuffer(SpriteConf conf)
{
    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3
    };
    //unsigned short* indices = new unsigned short[6] { 0, 1, 2, 0, 2, 3 };

    //�C���f�b�N�X�o�b�t�@�̐ݒ�
    numIndices = sizeof(indices) / sizeof(unsigned short);
    IndexBufferConf indexBufferConf = {};
    indexBufferConf.device = conf.device;
    indexBufferConf.size = sizeof(indices) * numIndices;// 4 bytes * �v�f�� indices
    indexBufferConf.stride = sizeof(unsigned short);
    indexBufferConf.count = numIndices;

    //������
    indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->init(indexBufferConf);
    //�R�s�[
    indexBuffer->copy(static_cast<uint16_t*>(indices));
}
