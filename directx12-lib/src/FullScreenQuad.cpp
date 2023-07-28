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
    this->num_indices_ = 4;
}

/// <summary>
/// �`�揈��
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
/// <param name="osrt">�I�t�X�N���[�������_�[�^�[�Q�b�g</param>
void FullScreenQuad::draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* crt)
{
    //���[�g�V�O�l�`����ݒ�
    rc->setRootSignature(this->root_signature_.get());
    //�p�C�v���C���X�e�[�g��ݒ�
    rc->setPipelineState(this->pso_.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //���_�o�b�t�@��ݒ�
    rc->setVertexBuffer(this->vertex_buffer_.get());
    //�f�B�X�N���v�^�쐬
    this->createSRV(device, crt);
    //SRV�q�[�v��ݒ�
    rc->setDescriptorHeap(this->srv_heap_.Get());
    //GPU�n���h����commandlist�ɐݒ�
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = this->srv_heap_->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpuHandle);

    //�h���[�R�[��
    rc->drawInstanced(this->num_indices_);
}

/// <summary>
/// Basic�V�F�[�_�[�̃y�A����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void FullScreenQuad::createShader(ID3D12Device* device)
{
    {//���_�V�F�[�_�[
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        conf.entryFuncName = "VSMain";
        conf.filePath = "./src/shaders/FullScreenQuadVS.hlsl";
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {//�s�N�Z���V�F�[�_�[
        ShaderConf conf = {};
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        conf.entryFuncName = "PSMain";
        conf.filePath = "./src/shaders/FullScreenQuadPS.hlsl";
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// ���_�o�b�t�@�̍쐬
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
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
    vertex_buffer_ = std::make_shared<VertexBuffer>();
    vertex_buffer_->init(conf);
    vertex_buffer_->copy(vertices);
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

    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�쐬
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void FullScreenQuad::createPipelineState(ID3D12Device* device)
{
    // �C���v�b�g���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
      { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
    };

    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐ݒ�
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
    desc.pRootSignature = this->root_signature_->getRootSignature();
    desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
    desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
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

    PipelineStateObject::PipelineStateObjectConf conf;
    conf.desc = desc;

    this->pso_ = PSOCacheManager::getInstance().getPSO(device, conf);
}

void FullScreenQuad::createSRVHeap(ID3D12Device* device)
{
    //SRV�p�̃f�B�X�N���v�^�q�[�v���쐬
    D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
    srv_heap_desc.NumDescriptors = 1;
    srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&srv_heap_desc, IID_PPV_ARGS(&this->srv_heap_)))) {
        throw new std::exception("failed to create SRV heap");
    }
}

void FullScreenQuad::createSRV(ID3D12Device* device, CompositeRenderTarget* osrt)
{
    //SRV�̍쐬
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv_desc.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(osrt->getResource(), &srv_desc, this->srv_heap_->GetCPUDescriptorHandleForHeapStart());
}

