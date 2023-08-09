#include "Model.h"

#ifdef _DEBUG
#pragma comment(lib,"assimp-vc142-mtd.lib")
#else
#pragma comment(lib,"assimp-vc142-md.lib")
#endif // _DEBUG


#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "IndexBufferFactory.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "DepthStencilCacheManager.h"
#include "ShaderCacheManager.h"
#include "ConstantBufferFactory.h"
#include "TextureCacheManager.h"
#include "RenderContext.h"
#include "CommonGraphicsConfig.h"

#include <stdexcept>
#include <math.h>

void Model::init(ID3D12Device* device, const char* model_file_path)
{
    Assimp::Importer importer;

    //TODO:���f���t�@�C���̓ǂݍ��݂��x���̂ŁA�ʃX���b�h�œǂݍ��ނ悤�ɂ���
    //TODO:�L���b�V�������

    //���f���t�@�C����ǂݍ���
    int flag = 0;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_JoinIdenticalVertices;
    flag |= aiProcess_SortByPType;
    const aiScene* scene = importer.ReadFile(model_file_path, flag);
    //�ǂݍ��݂Ɏ��s������G���[��f��
    if (!scene) {
        const std::string err = importer.GetErrorString();
        throw std::runtime_error("Failed to load model file." + err);
    }

    int mesh_num = scene->mNumMeshes;
    for (int i = 0; i < mesh_num; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        parseNode(scene, mesh);
    }

    this->initRootSignature(device);
    this->initDescriptorHeap(device);
    this->loadShader();
    this->initPipelineStateObject(device);
    this->initVertexBuffer(device);
    this->initIndexBuffer(device);
    this->initConstantBuffer(device);
    //this->initTexture(device, texture_file_path);
    this->initOffScreenRenderTarget(device);
    this->initDepthStencil(device);
}

void Model::update()
{
    this->constant_buffer_->copy(&this->conf_);
}

void Model::draw(RenderContext* rc)
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
    //�f�B�X�N���v�^�q�[�v��ݒ�
    rc->setDescriptorHeap(this->srv_cbv_uav_descriptor_heap_.get());
    rc->setGraphicsRootDescriptorTable(0, this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    //rc->setGraphicsRootDescriptorTable(1, this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    //�h���[�R�[��
    rc->drawIndexed(this->num_indices_);

    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̏������݂��I������B
    this->off_screen_render_target_->endRender(rc);
    OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

void Model::parseNode(const aiScene* scene, const aiMesh* mesh)
{
    //���_���
    this->vertices_.resize(mesh->mNumVertices);
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        ModelData::MeshVertex vertex = {};

        //���_���W
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        //uv���W
        if (mesh->mTextureCoords[0])
        {
            vertex.uv.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
            vertex.uv.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
        }


        if (mesh->HasNormals())
        {
            //�@���x�N�g��
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->HasTangentsAndBitangents())
        {
            //�ڐ��x�N�g��
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
        }

        //TODO:���_�F���擾����

        this->vertices_[i] = vertex;
    }

    //�C���f�b�N�X���
    faces_.resize(mesh->mNumFaces);
    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        this->faces_[i].index[0] = mesh->mFaces[i].mIndices[0];
        this->faces_[i].index[1] = mesh->mFaces[i].mIndices[1];
        this->faces_[i].index[2] = mesh->mFaces[i].mIndices[2];
    }

    return;
}

/// <summary>
/// ���[�g�V�O�l�`���̏�����
/// </summary>
/// <param name="device"></param>
void Model::initRootSignature(ID3D12Device* device)
{
    RootSignature::RootSignatureConf rootSignatureConf = {};
    rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.max_cbv_descriptor = 1;
    rootSignatureConf.max_srv_descriptor = 1;
    rootSignatureConf.offset_in_descriptors_from_table_start_cb = 0;
    rootSignatureConf.offset_in_descriptors_from_table_start_srv = 1;
    rootSignatureConf.num_sampler = 1;
    rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
    rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
    this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void Model::initDescriptorHeap(ID3D12Device* device)
{
    this->srv_cbv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
}

/// <summary>
/// �V�F�[�_�[�̓ǂݍ���
/// </summary>
void Model::loadShader()
{
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "VSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
        conf.file_path = "./src/shaders/3dModelVS.hlsl";
        this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
    {
        Shader::ShaderConf conf = {};
        conf.entry_func_name = "PSMain";
        conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
        conf.file_path = "./src/shaders/3dModelPS.hlsl";
        this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̏�����
/// </summary>
/// <param name="device"></param>
void Model::initPipelineStateObject(ID3D12Device* device)
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
        //{"COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
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

    //���X�^���C�U�[�X�e�[�g
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
void Model::initVertexBuffer(ID3D12Device* device)
{
    //���_�o�b�t�@�̐ݒ�
    VertexBuffer::VertexBufferConf conf = {};
    conf.size = sizeof(ModelData::MeshVertex) * this->vertices_.size();
    conf.stride = sizeof(ModelData::MeshVertex);

    //������
    this->vertex_buffer_ = VertexBufferFactory::create(conf, device);
    //�R�s�[
    this->vertex_buffer_->copy(this->vertices_.data());
}

/// <summary>
/// �C���f�b�N�X�o�b�t�@�̏�����
/// </summary>
/// <param name="device"></param>
void Model::initIndexBuffer(ID3D12Device* device)
{


    //�C���f�b�N�X�o�b�t�@�̐ݒ�
    this->num_indices_ = this->faces_.size() * 3;
    IndexBuffer::IndexBufferConf indexBufferConf = {};
    indexBufferConf.size = sizeof(ModelData::MeshFace) * this->faces_.size();// 4 bytes * �v�f�� indices
    indexBufferConf.stride = sizeof(UINT);
    indexBufferConf.count = this->num_indices_;

    //������
    this->index_buffer_ = IndexBufferFactory::create(indexBufferConf, device);
    //�R�s�[
    this->index_buffer_->copy(this->faces_.data());
}

/// <summary>
/// �萔�o�b�t�@�̏�����
/// </summary>
/// <param name="device"></param>
void Model::initConstantBuffer(ID3D12Device* device)
{
    ConstantBuffer::ConstantBufferConf conf = {};
    conf.size = sizeof(ModelConf);
    conf.descriptor_heap = this->srv_cbv_uav_descriptor_heap_.get();
    this->constant_buffer_ = ConstantBufferFactory::create(device, conf);
    constant_buffer_->copy(&this->conf_);
}

void Model::initTexture(ID3D12Device* device, const char* texture_file_path)
{
    this->texture_ = TextureCacheManager::getInstance().getOrCreate(device, this->srv_cbv_uav_descriptor_heap_.get(), texture_file_path);
}

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g�̏�����
/// </summary>
/// <param name="device"></param>
void Model::initOffScreenRenderTarget(ID3D12Device* device)
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
void Model::initDepthStencil(ID3D12Device* device)
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.frame_buffer_count = 1;
    ds_conf.width = windowWidth;
    ds_conf.height = windowHeight;
    this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}