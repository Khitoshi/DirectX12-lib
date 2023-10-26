#include "Sprite.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "OffScreenRenderTargetFactory.h"
#include "IndexBufferFactory.h"
#include "VertexBufferFactory.h"
#include "RootSignatureFactory.h"

#include "DepthStencilCacheManager.h"

#include "RenderContext.h"
#include "PSOCacheManager.h"
#include "ShaderCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "TextureCacheManager.h"

void Sprite::init(ID3D12Device* device, const char* texture_file_path)
{
	initRootSignature(device);
	initDescriptorHeap(device);
	initShader();
	initPipelineStateObject(device);
	initVertexBuffer(device);
	initIndexBuffer(device);
	initTexture(device, texture_file_path);
	initOffScreenRenderTarget(device);
	initDepthStencil(device);
}

void Sprite::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());


	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_.get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rc->setVertexBuffer(this->vertex_buffer_.get());
	rc->setIndexBuffer(this->index_buffer_.get());
	rc->setDescriptorHeap(this->descriptor_heap_.get());
	rc->setGraphicsRootDescriptorTable(0, this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	rc->drawIndexed(this->num_indices_);

	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

void Sprite::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rootSignatureConf = {};

	rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.num_sampler = 1;
	rootSignatureConf.num_srv = 1;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void Sprite::initDescriptorHeap(ID3D12Device* device)
{
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

void Sprite::initShader()
{
	{
		Shader::ShaderConf conf = {};
		conf.file_path = "./src/shaders/SpriteVS.hlsl";
		conf.entry_func_name = "VSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
	{
		Shader::ShaderConf conf = {};
		conf.file_path = "./src/shaders/SpritePS.hlsl";
		conf.entry_func_name = "PSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}


void Sprite::initPipelineStateObject(ID3D12Device* device)
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
	};

	// インプットレイアウト
	PipelineStateObject::PipelineStateObjectConf conf = {};
	conf.desc.pRootSignature = this->root_signature_->getRootSignature();
	conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
	conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
	conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	conf.desc.SampleMask = UINT_MAX;
	conf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	conf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

void Sprite::initVertexBuffer(ID3D12Device* device)
{
	//頂点データ
	this->vertices_[0] = {
		{ -0.75f, -0.75f, 0.0f }, {0,1}, // 左下
	};
	this->vertices_[1] = {
		{ -0.75f,  0.75f, 0.0f }, {0,0}, // 左上
	};
	this->vertices_[2] = {
		{  0.75f,  0.75f, 0.0f }, {1,0}, // 右上
	};
	this->vertices_[3] = {
		{  0.75f, -0.75f, 0.0f }, {1,1}, // 右下
	};

	//頂点バッファの設定
	VertexBuffer::VertexBufferConf conf = {};
	conf.size = sizeof(this->vertices_);
	conf.stride = sizeof(Vertex);

	//初期化
	this->vertex_buffer_ = VertexBufferFactory::create(conf, device);
	//コピー
	this->vertex_buffer_->map(this->vertices_, 4);
}

void Sprite::initIndexBuffer(ID3D12Device* device)
{
	//インデックスデータ
	unsigned short indices[] = {
		0, 1, 2, 0, 2, 3
	};

	//インデックスバッファの設定
	this->num_indices_ = sizeof(indices) / sizeof(unsigned short);
	IndexBuffer::IndexBufferConf indexBufferConf = {};
	indexBufferConf.size = sizeof(indices) * this->num_indices_;// 4 bytes * 要素数 indices
	indexBufferConf.stride = sizeof(unsigned short);
	indexBufferConf.count = this->num_indices_;

	//初期化
	this->index_buffer_ = IndexBufferFactory::create(indexBufferConf, device);
	//コピー
	this->index_buffer_->map(static_cast<uint16_t*>(indices), 6);
}

void Sprite::initTexture(ID3D12Device* device, const char* texture_file_path)
{
	//テクスチャの初期化
	this->texture_ = TextureCacheManager::getInstance().getOrCreate(device, texture_file_path);
	//ディスクリプタヒープに登録
	this->texture_->createSRV(device, this->descriptor_heap_.get(), 0);
}

void Sprite::initOffScreenRenderTarget(ID3D12Device* device)
{
	OffScreenRenderTarget::OffScreenRenderTargetConf osrtConf = {};

	{//ディスクリプタヒープの設定
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		osrtConf.descriptor_heap_desc = desc;
	}
	{//リソースデスクの設定
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

void Sprite::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count;
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void Sprite::setVertices(Vertex vertices[4])
{
	this->vertices_[0] = vertices[0];
	this->vertices_[1] = vertices[1];
	this->vertices_[2] = vertices[2];
	this->vertices_[3] = vertices[3];
	this->vertex_buffer_->map(this->vertices_, 4);
}

void Sprite::setTexture(ID3D12Device* device, const char* texture_file_path)
{
	this->texture_ = TextureCacheManager::getInstance().getOrCreate(device, texture_file_path);
	this->texture_->createSRV(device, this->descriptor_heap_.get(), 0);
}
