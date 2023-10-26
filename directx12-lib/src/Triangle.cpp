#include "Triangle.h"

#include "RenderContext.h"
#include "ShaderCacheManager.h"

#include "RootSignatureFactory.h"
#include "VertexBufferFactory.h"
#include "DepthStencilCacheManager.h"
#include "PSOCacheManager.h"

#include "OffScreenRenderTargetCacheManager.h"
#include "OffScreenRenderTargetFactory.h"
#include "IndexBufferFactory.h"

#include "GraphicsConfigurator.h"

void Triangle::init(ID3D12Device* device)
{
	initRootSignature(device);
	loadShader();
	initPipelineStateObject(device);
	initVertexBuffer(device);
	initIndexBuffer(device);
	initOffScreenRenderTarget(device);
	initDepthStencil(device);

	this->device_ = device;
}

void Triangle::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_[(int)this->render_mode_].get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rc->setVertexBuffer(this->vertex_buffer_.get());
	rc->setIndexBuffer(this->index_buffer_.get());
	rc->drawIndexed(3);

	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(this->off_screen_render_target_.get());
}

void Triangle::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rs_conf = {};
	rs_conf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	this->root_signature_ = RootSignatureFactory::create(device, rs_conf);
}


void Triangle::loadShader()
{
	{
		Shader::ShaderConf conf = {};
		conf.file_path = "./src/shaders/TriangleVS.hlsl";
		conf.entry_func_name = "VSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}

	{
		Shader::ShaderConf conf = {};
		conf.file_path = "./src/shaders/TrianglePS.hlsl";
		conf.entry_func_name = "PSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}

void Triangle::initPipelineStateObject(ID3D12Device* device)
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
	  { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
	};

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

	PipelineStateObject::PipelineStateObjectConf psoConf = {};
	psoConf.desc = desc;
	this->pso_[(int)RenderMode::Solid] = PSOCacheManager::getInstance().getOrCreate(device, psoConf);

	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	desc.RasterizerState = rasterizerDesc;
	psoConf.desc = desc;
	this->pso_[(int)RenderMode::WireFrame] = PSOCacheManager::getInstance().getOrCreate(device, psoConf);
}

void Triangle::initVertexBuffer(ID3D12Device* device)
{
	//���_�f�[�^
	this->vertices_[0] = {
		{-0.5f, -0.5f,  0.0f},
		{1.0f,  0.0f,   0.0f,  1.0f},
	};
	this->vertices_[1] = {
		{ 0.0f, 0.5f, 0.0f },
		{ 0.0f, 1.0f, 0.0f ,1.0f},
	};
	this->vertices_[2] = {
		{ 0.5f, -0.5f,  0.0f },
		{ 0.0f, 0.0f,   1.0f ,  1.0f},
	};

	VertexBuffer::VertexBufferConf vb_conf = {};
	vb_conf.size = sizeof(this->vertices_);
	vb_conf.stride = sizeof(Vertex);

	this->vertex_buffer_ = VertexBufferFactory::create(vb_conf, device);
	this->vertex_buffer_->map(this->vertices_, 3);
}

void Triangle::initIndexBuffer(ID3D12Device* device)
{
	unsigned short indices[] = {
		0,1,2
	};

	const int numIndices = sizeof(indices) / sizeof(unsigned short);
	IndexBuffer::IndexBufferConf indexBufferConf = {};
	indexBufferConf.size = sizeof(unsigned short) * numIndices;// 4 bytes * �v�f�� indices
	indexBufferConf.stride = sizeof(unsigned short);
	indexBufferConf.count = numIndices;

	this->index_buffer_ = IndexBufferFactory::create(indexBufferConf, device);
	this->index_buffer_->map(indices, numIndices);
}

void Triangle::initOffScreenRenderTarget(ID3D12Device* device)
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

void Triangle::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count;
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void Triangle::setVertices(Vertex vertices[3])
{
	this->vertices_[0] = vertices[0];
	this->vertices_[1] = vertices[1];
	this->vertices_[2] = vertices[2];
	this->vertex_buffer_->map(this->vertices_, 3);
}