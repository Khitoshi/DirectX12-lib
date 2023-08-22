#include "CubeModel.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "IndexBufferFactory.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "DepthStencilCacheManager.h"
#include "ShaderCacheManager.h"
#include "ConstantBufferFactory.h"
#include "RenderContext.h"
#include "CommonGraphicsConfig.h"

/// <summary>
/// ����������
/// </summary>
/// <param name="device"></param>
void CubeModel::init(ID3D12Device* device)
{
    this->loadShader();
    this->initRootSignature(device);
    this->initDescriptorHeap(device);
    this->initPipelineStateObject(device);
    this->initVertexBuffer(device);
    this->initIndexBuffer(device);
    this->initConstantBuffer(device);
    this->initOffScreenRenderTarget(device);
    this->initDepthStencil(device);
}

/// <summary>
/// �X�V����
/// </summary>
void CubeModel::update()
{
    this->constant_buffer_->copy(&this->conf_);
}

/// <summary>
/// �`�揈��
/// </summary>
/// <param name="rc"></param>
void CubeModel::draw(RenderContext* rc)
{
    {
        //�I�t�X�N���[�������_�[�^�[�Q�b�g�ŏ������݂ł����Ԃɂ���
        auto renderTarget = this->off_screen_render_target_->getRTVHeap();
        auto resource = this->off_screen_render_target_->getResource();
        auto depth_stencil = this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        //�r���[�|�[�g�ƃV�U�����O��`�̐ݒ�
        rc->transitionOffScreenRenderTargetBegin(resource);
        rc->simpleStart(renderTarget->GetCPUDescriptorHandleForHeapStart(), depth_stencil);
    }

    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->root_signature_.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->pso_.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vertex_buffer_.get());
    //�C���f�b�N�X�o�b�t�@��ݒ�
    rc->setIndexBuffer(this->index_buffer_.get());

    //�萔�o�b�t�@��ݒ�
    //rc->setConstantBufferView(this->constant_buffer_.get());
    std::vector<DescriptorHeap*>ds;
    ds.push_back(this->descriptor_heap_.get());
    rc->setDescriptorHeaps(ds);
    rc->setGraphicsRootDescriptorTable(0, this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    //�h���[�R�[��
    rc->drawIndexed(this->num_indices_);

    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̏������݂��I������B
    this->off_screen_render_target_->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

/// <summary>
/// ���[�g�V�O�l�`���̏�����
/// </summary>
/// <param name="device"></param>
void CubeModel::initRootSignature(ID3D12Device* device)
{
    RootSignature::RootSignatureConf rootSignatureConf = {};

    rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.num_cbv = 1;
    rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̍쐬
/// </summary>
/// <param name="device"></param>
void CubeModel::initDescriptorHeap(ID3D12Device* device)
{
    this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

/// <summary>
/// �V�F�[�_�[�̓ǂݍ���
/// </summary>
void CubeModel::loadShader()
{
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "VSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
        conf.file_path = "./src/shaders/CubeVS.hlsl";
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "PSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
        conf.file_path = "./src/shaders/CubePS.hlsl";
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̏�����
/// </summary>
/// <param name="device"></param>
void CubeModel::initPipelineStateObject(ID3D12Device* device)
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
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

    D3D12_RASTERIZER_DESC rasterizer_desc = {};
    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID; // �\���b�h�\��
    rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE; // �J�����O�I�t
    //rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK; // �w�ʃJ�����O�I��
    rasterizer_desc.FrontCounterClockwise = FALSE; // �ʏ�̏���
    rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


    // �C���v�b�g���C�A�E�g
    PipelineStateObject::PipelineStateObjectConf conf = {};
    conf.desc.pRootSignature = this->root_signature_->getRootSignature();
    conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
    conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
    conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    conf.desc.SampleMask = UINT_MAX;
    conf.desc.RasterizerState = rasterizer_desc;
    conf.desc.DepthStencilState = ds_desc;
    conf.desc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    conf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    conf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    conf.desc.NumRenderTargets = 1;
    conf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    conf.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    conf.desc.SampleDesc.Count = 1;
    conf.desc.SampleDesc.Quality = 0;
    conf.desc.NodeMask = 1;
    conf.desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    this->pso_ = PSOCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// ���_�o�b�t�@�̏�����
/// </summary>
/// <param name="device"></param>
void CubeModel::initVertexBuffer(ID3D12Device* device)
{
    //���_�f�[�^
    this->vertices_[0] = { {-0.75f, -0.75f, 0.0f }, { 0, 1 } };    // �����O
    this->vertices_[1] = { {-0.75f, 0.75f,  0.0f }, { 0, 0 } };     // ����O
    this->vertices_[2] = { {0.75f,  0.75f,  0.0f }, { 1, 0 } };      // �E��O
    this->vertices_[3] = { {0.75f,  -0.75f, 0.0f }, { 1, 1 } };     // �E���O
    this->vertices_[4] = { {-0.75f, -0.75f, -1.0f },{ 0, 1 } };   // ������
    this->vertices_[5] = { {-0.75f, 0.75f,  -1.0f },{ 0, 0 } };    // ���㉜
    this->vertices_[6] = { {0.75f,  0.75f,  -1.0f },{ 1, 0 } };     // �E�㉜
    this->vertices_[7] = { {0.75f,  -0.75f, -1.0f },{ 1, 1 } };    // �E����

    //���_�o�b�t�@�̐ݒ�
    VertexBuffer::VertexBufferConf conf = {};
    conf.size = sizeof(this->vertices_);
    conf.stride = sizeof(Vertex);

    //������
    this->vertex_buffer_ = VertexBufferFactory::create(conf, device);
    //�R�s�[
    this->vertex_buffer_->copy(this->vertices_);
}

/// <summary>
/// �C���f�b�N�X�o�b�t�@�̏�����
/// </summary>
/// <param name="device"></param>
void CubeModel::initIndexBuffer(ID3D12Device* device)
{
    //�C���f�b�N�X�f�[�^
    unsigned short indices[] = {
    0, 1, 2, 2, 3, 0, // �O��
    4, 5, 6, 6, 7, 4, // �w��
    0, 1, 5, 5, 4, 0, // ������
    2, 3, 7, 7, 6, 2, // �E����
    1, 2, 6, 6, 5, 1, // ���
    0, 3, 7, 7, 4, 0  // ����
    };

    //�C���f�b�N�X�o�b�t�@�̐ݒ�
    this->num_indices_ = sizeof(indices) / sizeof(unsigned short);
    IndexBuffer::IndexBufferConf indexBufferConf = {};
    indexBufferConf.size = sizeof(indices) * this->num_indices_;// 4 bytes * �v�f�� indices
    indexBufferConf.stride = sizeof(unsigned short);
    indexBufferConf.count = this->num_indices_;

    //������
    this->index_buffer_ = IndexBufferFactory::create(indexBufferConf, device);
    //�R�s�[
    this->index_buffer_->copy(static_cast<uint16_t*>(indices));
}

/// <summary>
/// �萔�o�b�t�@�̏�����
/// </summary>
/// <param name="device"></param>
void CubeModel::initConstantBuffer(ID3D12Device* device)
{
    ConstantBuffer::ConstantBufferConf conf = {};
    conf.size = sizeof(CubeModelConf);
    conf.descriptor_heap = this->descriptor_heap_.get();
    conf.slot = 0;
    this->constant_buffer_ = ConstantBufferFactory::create(device, conf);
    constant_buffer_->copy(&this->conf_);
}

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g�̏�����
/// </summary>
/// <param name="device"></param>
void CubeModel::initOffScreenRenderTarget(ID3D12Device* device)
{
    OffScreenRenderTarget::OffScreenRenderTargetConf osrtConf = {};

    {//�f�B�X�N���v�^�q�[�v�̐ݒ�
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = 1;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0;

        osrtConf.descriptor_heap_desc = desc;
    }
    {//���\�[�X�f�X�N�̐ݒ�
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = windowWidth;
        desc.Height = windowHeight;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        osrtConf.resource_desc = desc;
    }

    this->off_screen_render_target_ = OffScreenRenderTargetFactory::create(osrtConf, device);
}

/// <summary>
/// �[�x�X�e���V���̏�����
/// </summary>
/// <param name="device"></param>
void CubeModel::initDepthStencil(ID3D12Device* device)
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.frame_buffer_count = 1;
    ds_conf.width = windowWidth;
    ds_conf.height = windowHeight;
    this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}