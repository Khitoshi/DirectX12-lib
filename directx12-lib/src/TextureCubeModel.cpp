#include "TextureCubeModel.h"

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
#include "GraphicsConfigurator.h"

void TextureCubeModel::init(ID3D12Device* device, const char* texture_file_path)
{
	this->initRootSignature(device);
	this->initDescriptorHeap(device);
	this->loadShader();
	this->initPipelineStateObject(device);
	this->initVertexBuffer(device);
	this->initIndexBuffer(device);
	this->initConstantBuffer(device);
	this->initTexture(device, texture_file_path);
	this->initOffScreenRenderTarget(device);
	this->initDepthStencil(device);
}

void TextureCubeModel::update()
{
	this->constant_buffer_->map(&this->conf_, 1);
}

void TextureCubeModel::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_.get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rc->setVertexBuffer(this->vertex_buffer_.get());
	rc->setIndexBuffer(this->index_buffer_.get());

	//ディスクリプタヒープの設定
	rc->setDescriptorHeap(this->srv_cbv_uav_descriptor_heap_.get());
	auto handle = this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	rc->setGraphicsRootDescriptorTable(0, handle);
	handle.ptr += texture_descriptor_index_;
	rc->setGraphicsRootDescriptorTable(1, handle);

	rc->drawIndexed(this->num_indices_);

	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}


void TextureCubeModel::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rootSignatureConf = {};
	rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.num_cbv = 1;
	rootSignatureConf.num_srv = 1;
	rootSignatureConf.num_sampler = 1;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
	rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;

	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void TextureCubeModel::initDescriptorHeap(ID3D12Device* device)
{
	this->srv_cbv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
}

void TextureCubeModel::loadShader()
{
	{
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "VSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		conf.file_path = "./src/shaders/TextureCubeVS.hlsl";
		this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
	{
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "PSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		conf.file_path = "./src/shaders/TextureCubePS.hlsl";
		this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}

void TextureCubeModel::initPipelineStateObject(ID3D12Device* device)
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
	};

	//深度ステンシルステート
	D3D12_DEPTH_STENCIL_DESC ds_desc = {};
	//ds_desc.DepthEnable = FALSE; // 深度テストを無効にする
	ds_desc.DepthEnable = TRUE; // 深度テストを無効にする
	//ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;   // 深度値の書き込みを無効にする
	ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;       // 深度比較関数を常に真にする
	ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // 新しいフラグメントが現在の深度よりも小さい場合に描画
	ds_desc.StencilEnable = FALSE;
	ds_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	ds_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	//ラスタライザーステート
	D3D12_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID; // ソリッド表示
	rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE; // カリングオフ
	//rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK; // 背面カリングオン
	rasterizer_desc.FrontCounterClockwise = FALSE; // 通常の順序
	rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	rasterizer_desc.ForcedSampleCount = 0;
	rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// インプットレイアウト
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

void TextureCubeModel::initVertexBuffer(ID3D12Device* device)
{
	//頂点データ
	this->vertices_[0] = { {-0.75f, -0.75f, 0.0f }, { 0, 1 } };    // 左下前
	this->vertices_[1] = { {-0.75f, 0.75f,  0.0f }, { 0, 0 } };     // 左上前
	this->vertices_[2] = { {0.75f,  0.75f,  0.0f }, { 1, 0 } };      // 右上前
	this->vertices_[3] = { {0.75f,  -0.75f, 0.0f }, { 1, 1 } };     // 右下前
	this->vertices_[4] = { {-0.75f, -0.75f, -1.0f },{ 0, 1 } };   // 左下奥
	this->vertices_[5] = { {-0.75f, 0.75f,  -1.0f },{ 0, 0 } };    // 左上奥
	this->vertices_[6] = { {0.75f,  0.75f,  -1.0f },{ 1, 0 } };     // 右上奥
	this->vertices_[7] = { {0.75f,  -0.75f, -1.0f },{ 1, 1 } };    // 右下奥

	//頂点バッファの設定
	VertexBuffer::VertexBufferConf conf = {};
	conf.size = sizeof(this->vertices_);
	conf.stride = sizeof(Vertex);

	//初期化
	this->vertex_buffer_ = VertexBufferFactory::create(conf, device);
	//コピー
	this->vertex_buffer_->map(this->vertices_, 8);
}

void TextureCubeModel::initIndexBuffer(ID3D12Device* device)
{
	//インデックスデータ
	unsigned short indices[] = {
	0, 1, 2, 2, 3, 0, // 前面
	4, 5, 6, 6, 7, 4, // 背面
	0, 1, 5, 5, 4, 0, // 左側面
	2, 3, 7, 7, 6, 2, // 右側面
	1, 2, 6, 6, 5, 1, // 上面
	0, 3, 7, 7, 4, 0  // 下面
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
	this->index_buffer_->map(static_cast<uint16_t*>(indices), this->num_indices_);
}

void TextureCubeModel::initConstantBuffer(ID3D12Device* device)
{
	ConstantBuffer::ConstantBufferConf conf = {};
	conf.size = sizeof(TextureCubeModelConf);
	conf.descriptor_heap = this->srv_cbv_uav_descriptor_heap_.get();
	conf.slot = 0;
	this->constant_buffer_ = ConstantBufferFactory::create(device, conf);
	this->constant_buffer_->map(&this->conf_, 1);
}

void TextureCubeModel::initTexture(ID3D12Device* device, const char* texture_file_path)
{
	//テクスチャの初期化
	this->texture_ = TextureCacheManager::getInstance().getOrCreate(device, texture_file_path);
	//ディスクリプタヒープに登録
	this->texture_->createSRV(device, this->srv_cbv_uav_descriptor_heap_.get(), 1);
	texture_descriptor_index_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 1;
}

void TextureCubeModel::initOffScreenRenderTarget(ID3D12Device* device)
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

void TextureCubeModel::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = 1;
	ds_conf.width = GraphicsConfigurator::getWindowWidth();
	ds_conf.height = GraphicsConfigurator::getWindowHeight();
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}