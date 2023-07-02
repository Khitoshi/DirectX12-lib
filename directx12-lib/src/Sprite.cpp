#include "Sprite.h"

/// <summary>
/// ����������
/// </summary>
/// <param name="conf">�e�N�X�`���̐ݒ�</param>
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
/// �`�揈��
/// </summary>
/// <param name="rc"></param>
void Sprite::draw(RenderContext* rc)
{
    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->rootSignature.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->defaultPipelineStateObject.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertexBuffer.get());
    //�C���f�b�N�X�o�b�t�@��ݒ�B
    rc->setIndexBuffer(this->indexBuffer.get());

    //�e�N�X�`����ݒ�B
    rc->setTexture(this->texture.get());

    //�h���[�R�[��
    rc->drawIndexed(numIndices);
}

/// <summary>
/// ���[�g�V�O�j�`���̏�����
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
/// �V�F�[�_�[�̃��[�h(�R���p�C��)
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
/// pso�̏�����
/// </summary>
/// <param name="conf"></param>
void Sprite::initPipelineStateObject(SpriteConf conf)
{
    // �C���v�b�g���C�A�E�g
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
/// ���_�o�b�t�@�̏�����
/// </summary>
/// <param name="conf"></param>
void Sprite::initVertexBuffer(SpriteConf conf)
{
    //���_�f�[�^
    vertices[0] = {
        //{ -0.25f, -0.25f, 0.5f }, {0,1}, // ����
        { -0.75f, -0.75f, 0.0f }, {0,1}, // ����
    };
    vertices[1] = {
        //{ -0.25f,  0.25f, 0.5f }, {0,0}, // ����
        { -0.75f,  0.75f, 0.0f }, {0,0}, // ����
    };
    vertices[2] = {
        //{  0.25f,  0.25f, 0.5f }, {1,0}, // �E��
        {  0.75f,  0.75f, 0.0f }, {1,0}, // �E��
    };
    vertices[3] = {
        //{  0.25f, -0.25f, 0.5f }, {1,1}, // �E��
        {  0.75f, -0.75f, 0.0f }, {1,1}, // �E��
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
/// <param name="conf"></param>
void Sprite::initIndexBuffer(SpriteConf conf)
{
    //�C���f�b�N�X�f�[�^
    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3
    };

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

/// <summary>
/// �e�N�X�`���̏�����
/// </summary>
/// <param name="conf"></param>
void Sprite::initTexture(SpriteConf conf)
{
    //�e�N�X�`���̏�����
    Texture::TextureConf textureConf = {};
    textureConf.device = conf.device;
    textureConf.filePath = conf.filePath;
    texture = std::make_shared<Texture>(textureConf);
    texture->Load();
}
