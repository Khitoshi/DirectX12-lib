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
#include "GraphicsConfigurator.h"


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

void CubeModel::update()
{
	this->constant_buffer_->map<CubeModelConf>(&this->conf_, 1);
}


void CubeModel::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_.get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rc->setVertexBuffer(this->vertex_buffer_.get());
	rc->setIndexBuffer(this->index_buffer_.get());

	//�萔�o�b�t�@��ݒ�
	std::vector<DescriptorHeap*>ds;
	ds.push_back(this->descriptor_heap_.get());
	rc->setDescriptorHeaps(ds);
	rc->setGraphicsRootDescriptorTable(0, this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	rc->drawIndexed(this->num_indices_);

	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

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

void CubeModel::initDescriptorHeap(ID3D12Device* device)
{
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

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

void CubeModel::initVertexBuffer(ID3D12Device* device)
{
	//���_�f�[�^
	this->vertices_[0] = { {-0.75f, -0.75f, 0.0f }, { 0, 1 } };// �����O
	this->vertices_[1] = { {-0.75f, 0.75f,  0.0f }, { 0, 0 } };// ����O
	this->vertices_[2] = { {0.75f,  0.75f,  0.0f }, { 1, 0 } };// �E��O
	this->vertices_[3] = { {0.75f,  -0.75f, 0.0f }, { 1, 1 } };// �E���O

	this->vertices_[4] = { {-0.75f, -0.75f, -1.0f },{ 0, 1 } };// ������
	this->vertices_[5] = { {-0.75f, 0.75f,  -1.0f },{ 0, 0 } };// ���㉜
	this->vertices_[6] = { {0.75f,  0.75f,  -1.0f },{ 1, 0 } };// �E�㉜
	this->vertices_[7] = { {0.75f,  -0.75f, -1.0f },{ 1, 1 } };// �E����

	VertexBuffer::VertexBufferConf conf = {};
	conf.size = sizeof(this->vertices_);
	conf.stride = sizeof(Vertex);

	this->vertex_buffer_ = VertexBufferFactory::create(conf, device);
	this->vertex_buffer_->map<Vertex>(this->vertices_, 8);
}

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

	this->num_indices_ = sizeof(indices) / sizeof(unsigned short);
	IndexBuffer::IndexBufferConf indexBufferConf = {};
	indexBufferConf.size = sizeof(indices) * this->num_indices_;// 4 bytes * �v�f�� indices
	indexBufferConf.stride = sizeof(unsigned short);
	indexBufferConf.count = this->num_indices_;

	this->index_buffer_ = IndexBufferFactory::create(indexBufferConf, device);
	this->index_buffer_->map(indices, this->num_indices_);
}

void CubeModel::initConstantBuffer(ID3D12Device* device)
{
	ConstantBuffer::ConstantBufferConf conf = {};
	conf.size = sizeof(CubeModelConf);
	conf.descriptor_heap = this->descriptor_heap_.get();
	conf.slot = 0;
	this->constant_buffer_ = ConstantBufferFactory::create(device, conf);
	constant_buffer_->map<CubeModelConf>(&this->conf_, 1);
}

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
		desc.Width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
		desc.Height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
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

void CubeModel::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = 1;
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}