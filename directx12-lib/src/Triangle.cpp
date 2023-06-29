#include "Triangle.h"

/// <summary>
/// �O�p�`�ɕK�v�ȃ��\�[�X�̏�����
/// </summary>
/// <param name="conf">�O�p�`�̕`��p�ݒ�</param>
void Triangle::init(TriangleConf conf)
{
    initRootSignature(conf);
    loadShader();
    initPipelineStateObject(conf);
    initVertexBuffer(conf);
    initIndexBuffer(conf);
}

/// <summary>
/// �O�p�`�`��
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
void Triangle::draw(RenderContext* rc)
{
    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->rootSignature.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->pipelineStateObject[(int)renderMode].get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertexBuffer.get());
    //�C���f�b�N�X�o�b�t�@��ݒ�B
    rc->setIndexBuffer(this->indexBuffer.get());
    //�h���[�R�[��
    rc->drawIndexed(3);
}

/// <summary>
/// ���[�g�V�O�j�`���̏�����
/// </summary>
/// <param name="conf">�O�p�`�̕`��p�ݒ�</param>
void Triangle::initRootSignature(TriangleConf conf)
{
    //���[�g�V�O�l�`���̐ݒ�(���W�X�^���g�p���Ȃ��̂ŋ�ɂ���)
    RootSignatureConf rootSignatureConf = {};
    rootSignatureConf.device = conf.device;
    rootSignatureConf.rootParameters = {};
    rootSignatureConf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureConf.samplerDescArray = {};
    rootSignature = std::make_shared<RootSignature>();
    rootSignature->init(rootSignatureConf);
}

/// <summary>
/// �V�F�[�_�[�̃��[�h(�R���p�C��)
/// </summary>
void Triangle::loadShader()
{
    //���_�V�F�[�_�[�̃��[�h
    vertexShader = std::make_shared<Shader>();
    ShaderConf vsConf = {};
    vsConf.filePath = "./src/shaders/TriangleVS.hlsl";
    vsConf.entryFuncName = "VSMain";
    vertexShader->LoadVS(vsConf);

    //�s�N�Z���V�F�[�_�[�̃��[�h
    pixelShader = std::make_shared<Shader>();
    ShaderConf psConf = {};
    psConf.filePath = "./src/shaders/TrianglePS.hlsl";
    psConf.entryFuncName = "PSMain";
    pixelShader->LoadPS(psConf);
}

/// <summary>
/// pso�̏�����
/// </summary>
/// <param name="conf">�O�p�`�̕`��p�ݒ�</param>
void Triangle::initPipelineStateObject(TriangleConf conf)
{
    // �C���v�b�g���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
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
    pipelineStateObject[(int)RenderMode::Solid] = std::make_shared<PipelineStateObject>();
    pipelineStateObject[(int)RenderMode::Solid]->init(psoConf);

    // ���C���[�t���[�����[�h������
    D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    desc.RasterizerState = rasterizerDesc;
    psoConf.desc = desc;
    pipelineStateObject[(int)RenderMode::WireFrame] = std::make_shared<PipelineStateObject>();
    pipelineStateObject[(int)RenderMode::WireFrame]->init(psoConf);

}

/// <summary>
/// ���_�o�b�t�@�̏�����
/// </summary>
/// <param name="conf">�O�p�`�̕`��p�ݒ�</param>
void Triangle::initVertexBuffer(TriangleConf conf)
{
    //���_�f�[�^
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

/// <summary>
/// �C���f�b�N�X�o�b�t�@�̏�����
/// </summary>
/// <param name="conf">�O�p�`�̕`��p�ݒ�</param>
void Triangle::initIndexBuffer(TriangleConf conf)
{
    unsigned short indices[] = {
        0,1,2
    };

    //�C���f�b�N�X�o�b�t�@�̐ݒ�
    const int numIndices = sizeof(indices) / sizeof(unsigned short);
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