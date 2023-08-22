#include "CompositeRenderTarget.h"
#include "DescriptorCache.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "ShaderCacheManager.h"
#include "CommonGraphicsConfig.h"
#include "VertexBufferFactory.h"
#include "RenderContext.h"
#include <stdexcept>

/// <summary>
/// ������
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void CompositeRenderTarget::init(ID3D12Device* device)
{
    createResource(device);
    createSRVHeap(device);
    createSRVDesc(device);
    createRTVHeap(device);
    createRTV(device);

    initRootSignature(device);
    initShader(device);
    initVertexBuffer(device);
    initPipelineStateObject(device);
    initDescriptorHeapCache();
}

/// <summary>
/// �`�揉������
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
/// <param name="depthStencilViewHandle">�[�x�X�e���V���r���[�̃n���h��</param>
void CompositeRenderTarget::beginRender(RenderContext* rc, D3D12_CPU_DESCRIPTOR_HANDLE depthStencil_view_handle)
{
    //���\�[�X�̏�Ԃ�J��
    rc->transitionOffScreenRenderTargetBegin(this->resource_.Get());
    //�`��J�n����
    rc->simpleStart(this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), depthStencil_view_handle);
}

/// <summary>
/// �`�揈��
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::render(RenderContext* rc, ID3D12Device* device)
{
    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->root_signature_.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->pso_.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertex_buffer_.get());

    // SRVHeap�̎n�_���擾
    UINT slot = 0;
    for (auto& rt : OffScreenRenderTargetCacheManager::getInstance().getRenderTargetList())
    {
        //�f�B�X�N���v�^�̐���
        DescriptorCache::DescriptorCacheConf dhcConf = {};
        dhcConf.resource = rt->getResource();
        dhcConf.slot = slot;
        this->descriptor_cache_->getOrCreate(device, dhcConf, this->cbv_srv_uav_descriptor_heap_->getDescriptorHeap(), srv_desc_);
        slot++;
    }
    //�f�B�X�N���v�^�q�[�v��ݒ�
    rc->setDescriptorHeap(this->cbv_srv_uav_descriptor_heap_.get());

    //GPU�n���h����commandlist�ɐݒ�
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = this->cbv_srv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpu_handle);

    gpu_handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 1;
    rc->setGraphicsRootDescriptorTable(1, gpu_handle);

    //�h���[�R�[��
    rc->drawInstanced(4);
}

/// <summary>
/// offscreen�ɕ`�������ׂ̏I������
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
void CompositeRenderTarget::endRender(RenderContext* rc)
{
    //�I�t�X�N���[�����X�g���N���A
    OffScreenRenderTargetCacheManager::getInstance().clearRenderTargetList();
    //�����_�[�^�[�Q�b�g��RESOURCE_BARRIER�ݒ�
    rc->transitionOffScreenRenderTargetEnd(this->resource_.Get());
}

/// <summary>
/// ���\�[�X���쐬����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::createResource(ID3D12Device* device)
{
    D3D12_RESOURCE_DESC desc = {};
    desc = this->conf_.resource_desc;
    D3D12_HEAP_PROPERTIES heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_CLEAR_VALUE clear_value = {};
    clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, backGroundColor);
    if (FAILED(device->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clear_value,
        IID_PPV_ARGS(this->resource_.ReleaseAndGetAddressOf())
    ))) {
        throw std::runtime_error("failed to create offscreen render target");
    }
}

/// <summary>
/// �V�F�[�_�[���\�[�X�r���[�p�̃f�B�X�N���v�^�q�[�v���쐬����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::createSRVHeap(ID3D12Device* device)
{
    this->cbv_srv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
}

/// <summary>
/// �V�F�[�_�[���\�[�X�r���[���쐬����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::createSRVDesc(ID3D12Device* device)
{
    ////�V�F�[�_�[���\�[�X�r���[�̐ݒ�
    srv_desc_ = {};
    srv_desc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc_.Texture2D.MipLevels = 1;
    srv_desc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

/// <summary>
/// �����_�[�^�[�Q�b�g�r���[�p�̃f�B�X�N���v�^�q�[�v���쐬����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::createRTVHeap(ID3D12Device* device)
{
    this->rtv_descriptor_heap_ = DescriptorHeapFactory::create(device, this->conf_.descriptor_heap_desc.Type, 1);
}

/// <summary>
/// �����_�[�^�[�Q�b�g�r���[���쐬����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::createRTV(ID3D12Device* device)
{
    D3D12_RENDER_TARGET_VIEW_DESC desc = {};
    desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateRenderTargetView(this->resource_.Get(), &desc, this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

/// <summary>
/// ���[�g�V�O�j�`�����쐬����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::initRootSignature(ID3D12Device* device)
{
    RootSignature::RootSignatureConf rs_conf = {};
    rs_conf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rs_conf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rs_conf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rs_conf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rs_conf.num_sampler = 1;
    rs_conf.num_srv = 2;
    rs_conf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rs_conf);
}

/// <summary>
/// �V�F�[�_�[������������
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::initShader(ID3D12Device* device)
{
    //TODO :���I�ɃV�F�[�_�[���쐬����悤�ɂ���
    //�쐬�����V�F�[�_�[�̓L���b�V������
    {//���_�V�F�[�_�[���[�h
        Shader::ShaderConf conf = {};
        conf.file_path = "./src/shaders/MultiPathCompositingVS.hlsl";
        conf.entry_func_name = "VSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }

    {//�s�N�Z���V�F�[�_�[���[�h
        Shader::ShaderConf conf = {};
        conf.file_path = "./src/shaders/MultiPathCompositingPS.hlsl";
        conf.entry_func_name = "PSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̏�����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::initPipelineStateObject(ID3D12Device* device)
{
    //�C���v�b�g���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC input_rlement_desc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
    };

    //�[�x�X�e���V���X�e�[�g
    D3D12_DEPTH_STENCIL_DESC ds_desc = {};
    ds_desc.DepthEnable = FALSE; // �[�x�e�X�g�𖳌��ɂ���
    ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;   // �[�x�l�̏������݂𖳌��ɂ���
    ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;       // �[�x��r�֐�����ɐ^�ɂ���
    ds_desc.StencilEnable = FALSE;
    ds_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    ds_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    ds_desc.FrontFace = defaultStencilOp;
    ds_desc.BackFace = defaultStencilOp;

    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐ݒ�
    PipelineStateObject::PipelineStateObjectConf pso_conf = {};
    pso_conf.desc.pRootSignature = this->root_signature_->getRootSignature();
    pso_conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
    pso_conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
    pso_conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso_conf.desc.SampleMask = UINT_MAX;
    pso_conf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_conf.desc.DepthStencilState = ds_desc;
    pso_conf.desc.InputLayout = { input_rlement_desc, _countof(input_rlement_desc) };
    pso_conf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pso_conf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_conf.desc.NumRenderTargets = 1;
    pso_conf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso_conf.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pso_conf.desc.SampleDesc.Count = 1;
    pso_conf.desc.SampleDesc.Quality = 0;
    pso_conf.desc.NodeMask = 1;
    pso_conf.desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    this->pso_ = PSOCacheManager::getInstance().getOrCreate(device, pso_conf);
}

/// <summary>
/// ���_�o�b�t�@������������
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CompositeRenderTarget::initVertexBuffer(ID3D12Device* device)
{
    Vertex vertices[] = {
        {{ -1.0f, -1.0f, 0.0f }, {0,1}}, // ����
        {{ -1.0f,  1.0f, 0.0f }, {0,0}}, // ����
        {{  1.0f, -1.0f, 0.0f }, {1,1}}, // �E��
        {{  1.0f,  1.0f, 0.0f }, {1,0}}, // �E��
    };

    VertexBuffer::VertexBufferConf vb_conf = {};
    vb_conf.size = sizeof(vertices);
    vb_conf.stride = sizeof(Vertex);

    this->vertex_buffer_ = VertexBufferFactory::create(vb_conf, device);
    this->vertex_buffer_->copy(vertices);
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̃L���b�V����������
/// </summary>
void CompositeRenderTarget::initDescriptorHeapCache()
{
    this->descriptor_cache_ = std::make_unique<DescriptorCache>();
}

ID3D12DescriptorHeap* CompositeRenderTarget::getRTVHeap() const
{
    return rtv_descriptor_heap_->getDescriptorHeap();
}
