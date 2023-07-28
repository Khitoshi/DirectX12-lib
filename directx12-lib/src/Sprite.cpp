#include "Sprite.h"
//#include "ShaderCacheManager.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "OffScreenRenderTargetFactory.h"
#include "IndexBufferFactory.h"

/// <summary>
/// ����������
/// </summary>
/// <param name="conf">�e�N�X�`���̐ݒ�</param>
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
/// �`�揈��
/// </summary>
/// <param name="rc"></param>
void Sprite::draw(RenderContext* rc)
{
    {
        //�I�t�X�N���[�������_�[�^�[�Q�b�g�ŏ������݂ł����Ԃɂ���
        auto renderTarget = offScreenRenderTarget->getRTVHeap();
        auto resource = offScreenRenderTarget->getResource();
        auto depthStencil = OffScreenRenderTargetCacheManager::getInstance().getDepthStencilViewHandle();
        auto viewport = OffScreenRenderTargetCacheManager::getInstance().getViewport();

        //�r���[�|�[�g�ƃV�U�����O��`�̐ݒ�
        rc->transitionOffScreenRenderTargetBegin(resource);
        //rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depthStencil, resource);
        rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depthStencil);
    }


    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->rootSignature.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->defaultPipelineStateObject.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertexBuffer.get());
    //�C���f�b�N�X�o�b�t�@��ݒ�B
    rc->setIndexBuffer(this->indexBuffer.get());
    //�e�N�X�`����ݒ�B
    rc->setTexture(this->texture.get());

    //�h���[�R�[��
    rc->drawIndexed(this->numIndices);

    /*
    this->rotationEffect->update(rc, conf.camera, this->vertexBuffer.get(), this->indexBuffer.get(), this->numIndices);
    rc->setIndexBuffer(this->indexBuffer.get());
    rc->setVertexBuffer(this->vertexBuffer.get());
    */

    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̏������݂��I������B
    offScreenRenderTarget->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(offScreenRenderTarget.get());
}

/// <summary>
/// ���[�g�V�O�j�`���̏�����
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
/// �V�F�[�_�[�̃��[�h(�R���p�C��)
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
/// pso�̏�����
/// </summary>
/// <param name="conf"></param>
void Sprite::initPipelineStateObject()
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
    };

    // �C���v�b�g���C�A�E�g
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
/// ���_�o�b�t�@�̏�����
/// </summary>
/// <param name="conf"></param>
void Sprite::initVertexBuffer()
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
void Sprite::initIndexBuffer()
{
    //�C���f�b�N�X�f�[�^
    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3
    };

    //�C���f�b�N�X�o�b�t�@�̐ݒ�
    numIndices = sizeof(indices) / sizeof(unsigned short);
    IndexBuffer::IndexBufferConf indexBufferConf = {};
    indexBufferConf.size = sizeof(indices) * numIndices;// 4 bytes * �v�f�� indices
    indexBufferConf.stride = sizeof(unsigned short);
    indexBufferConf.count = numIndices;

    //������
    indexBuffer = IndexBufferFactory::create(indexBufferConf, conf.device);
    //�R�s�[
    indexBuffer->copy(static_cast<uint16_t*>(indices));
}

/// <summary>
/// �e�N�X�`���̏�����
/// </summary>
/// <param name="conf"></param>
void Sprite::initTexture()
{
    //�e�N�X�`���̏�����
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