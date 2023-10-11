#include "PhongShading.h"

#include "ShaderCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "ConstantBufferFactory.h"
#include "DepthStencilCacheManager.h"

#include "OffScreenRenderTargetCacheManager.h"

#include "RenderContext.h"
void PhongShading::init(ID3D12Device* device)
{

}

void PhongShading::update()
{
}

void PhongShading::draw(RenderContext* rc)
{
}


void PhongShading::loadShader()
{
	{//���_
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "VSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		conf.file_path = "./src/shaders/3dModelVS.hlsl";
		this->vs_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
	{//�s�N�Z��
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "PSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		conf.file_path = "./src/shaders/3dModelPS.hlsl";
		this->ps_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}

void PhongShading::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rootSignatureConf = {};
	rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.num_cbv = 2;
	rootSignatureConf.num_srv = 1;
	rootSignatureConf.num_sampler = 1;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
	rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void PhongShading::initDescriptorHeap(ID3D12Device* device)
{
	this->srv_cbv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

void PhongShading::initPipelineStateObject(ID3D12Device* device)
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
	};

	//�[�x�X�e���V���X�e�[�g
	D3D12_DEPTH_STENCIL_DESC ds_desc = {};
	//ds_desc.DepthEnable = FALSE; // �[�x�e�X�g�𖳌��ɂ���
	ds_desc.DepthEnable = TRUE; // �[�x�e�X�g�𖳌��ɂ���
	//ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;   // �[�x�l�̏������݂𖳌��ɂ���
	ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;       // �[�x��r�֐�����ɐ^�ɂ���
	ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // �V�����t���O�����g�����݂̐[�x�����������ꍇ�ɕ`��
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
	//rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE; // �J�����O�I�t
	//rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK; // �w�ʃJ�����O�I��
	//rasterizer_desc.CullMode = D3D12_CULL_MODE_FRONT;
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
	conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vs_->getShaderBlob());
	conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->ps_->getShaderBlob());
	conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	conf.desc.SampleMask = UINT_MAX;
	conf.desc.RasterizerState = rasterizer_desc;
	conf.desc.DepthStencilState = ds_desc;
	//conf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

void PhongShading::initVertexBuffer(ID3D12Device* device)
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	//������
	std::vector<Vertex> vertices(4);

	vertices[0] = Vertex({ -1.0f, -1.0f, 0.0f }, { 0,1 });// ����
	vertices[1] = Vertex({ -1.0f, 1.0f, 0.0f }, { 0,0 }); // ����
	vertices[2] = Vertex({ 1.0f, -1.0f, 0.0f }, { 1,1 }); // �E��
	vertices[3] = Vertex({ 1.0f,  1.0f, 0.0f }, { 1,0 }); // �E��

	VertexBuffer::VertexBufferConf vb_conf = {};
	vb_conf.size = static_cast<UINT>(vertices.size() * sizeof(Vertex));  // �C���_
	vb_conf.stride = sizeof(Vertex);

	this->vb_ = VertexBufferFactory::create(vb_conf, device);
	this->vb_->map(vertices.data(), static_cast<UINT>(vertices.size()));
}

void PhongShading::initConstantBuffer(ID3D12Device* device)
{
	ConstantBuffer::ConstantBufferConf conf = {};
	conf.size = sizeof(PhongConf);
	conf.descriptor_heap = this->srv_cbv_uav_descriptor_heap_.get();
	conf.slot = 0;
	this->cb_ = ConstantBufferFactory::create(device, conf);

	this->cb_->map(&this->conf_, 1);
}

void PhongShading::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = 1;
	ds_conf.width = GraphicsConfigurator::getWindowWidth();
	ds_conf.height = GraphicsConfigurator::getWindowHeight();
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void PhongShading::initOffScreenRenderTarget(ID3D12Device* device)
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
		desc.Width = GraphicsConfigurator::getWindowWidth();
		desc.Height = GraphicsConfigurator::getWindowHeight();
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





//TODO:init�ł͂Ȃ��̂Œ���
void PhongShading::initTexture(ID3D12Device* device, OffScreenRenderTarget* offscreen_rt)
{
	//�V�F�[�_�[���\�[�X�r���[�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_ = {};
	srv_desc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc_.Texture2D.MipLevels = 1;
	srv_desc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	device->CreateShaderResourceView(offscreen_rt->getResource(), &srv_desc_, handle);
}


