#include "FullScreenQuad.h"

/// <summary>
/// ����������
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void FullScreenQuad::init(ID3D12Device* device)
{
    createShader(device);
    createVertexBuffer(device);
    createRootSignature(device);
    createPipelineState(device);
    createSRVHeap(device);
    this->numIndices = 4;
}

/// <summary>
/// �`�揈��
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
/// <param name="osrt">�I�t�X�N���[�������_�[�^�[�Q�b�g</param>
void FullScreenQuad::draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* crt)
{
    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->rootSignature.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->pso.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertexBuffer.get());
    //�C���f�b�N�X�o�b�t�@��ݒ�B
    //rc->setTexture(crt->getSRVHeap(), 1);
    this->createSRV(device, crt);
    rc->setDescriptorHeap(this->SRVHeap.Get());
    //GPU�n���h����commandlist�ɐݒ�
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = this->SRVHeap->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpuHandle);

    //�h���[�R�[��
    rc->drawInstanced(this->numIndices);
}

/// <summary>
/// Basic�V�F�[�_�[�̃y�A����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void FullScreenQuad::createShader(ID3D12Device* device)
{
    {
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        conf.entryFuncName = "VSMain";
        conf.filePath = "./src/shaders/FullScreenQuadVS.hlsl";
        basicShaderPair.vertexShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        conf.entryFuncName = "PSMain";
        conf.filePath = "./src/shaders/FullScreenQuadPS.hlsl";
        basicShaderPair.pixelShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// ���_�o�b�t�@�̍쐬
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void FullScreenQuad::createVertexBuffer(ID3D12Device* device)
{
    //���_�o�b�t�@�̍쐬
    Vertex vertices[] = {
        {{ -1.0f, -1.0f, 0.0f }, {0,1}}, // ����
        {{ -1.0f,  1.0f, 0.0f }, {0,0}}, // ����
        {{  1.0f, -1.0f, 0.0f }, {1,1}}, // �E��
        {{  1.0f,  1.0f, 0.0f }, {1,0}}, // �E��
    };

    VertexBufferConf conf = {};
    conf.device = device;
    conf.size = sizeof(vertices);
    conf.stride = sizeof(Vertex);
    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->init(conf);
    vertexBuffer->copy(vertices);
}

/// <summary>
/// ���[�g�V�O�j�`���̍쐬
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void FullScreenQuad::createRootSignature(ID3D12Device* device)
{
    //���[�g�V�O�j�`���̐ݒ�(���W�X�^���g�p���Ȃ��̂ŋ�ɂ���)
    RootSignatureConf conf = {};
    conf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    conf.samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    conf.textureAddressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    conf.textureAddressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    conf.textureAddressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    conf.numSampler = 1;
    conf.maxSrvDescriptor = 1;
    conf.offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    this->rootSignature = RootSignatureCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�쐬
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void FullScreenQuad::createPipelineState(ID3D12Device* device)
{
    PipelineStateObject::PipelineStateObjectConf conf;
    // �C���v�b�g���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
    };

    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐ݒ�
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
    desc.pRootSignature = this->rootSignature->getRootSignature();
    desc.VS = CD3DX12_SHADER_BYTECODE(this->basicShaderPair.vertexShader->getShaderBlob());
    desc.PS = CD3DX12_SHADER_BYTECODE(this->basicShaderPair.pixelShader->getShaderBlob());
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
    conf.desc = desc;

    this->pso = PSOCacheManager::getInstance().getPSO(device, conf);
}

void FullScreenQuad::createSRVHeap(ID3D12Device* device)
{
    //SRV�p�̃f�B�X�N���v�^�q�[�v���쐬
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&this->SRVHeap)))) {
        throw new std::exception("failed to create SRV heap");
    }
}

void FullScreenQuad::createSRV(ID3D12Device* device, CompositeRenderTarget* osrt)
{
    //SRV�̍쐬
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(osrt->getResource(), &srvDesc, this->SRVHeap->GetCPUDescriptorHandleForHeapStart());
}

