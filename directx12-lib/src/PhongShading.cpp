#include "PhongShading.h"

#include "ShaderCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "ConstantBufferFactory.h"
#include "DepthStencilCacheManager.h"

#include "OffScreenRenderTargetCacheManager.h"
#include "GeometryBuffer.h"
#include "RenderContext.h"
#include <imgui/imgui.h>
#include "ImGuiManager.h"

void PhongShading::init(ID3D12Device* device)
{
	loadShader();
	initRootSignature(device);
	initDescriptorHeap(device);
	initPipelineStateObject(device);
	initVertexBuffer(device);
	initConstantBuffer(device);
	initDepthStencil(device);
	initOffScreenRenderTarget(device);
	this->device_ = device;
}

void PhongShading::update(
	Camera* camera,
	OffScreenRenderTarget* ort,
	GeometryBuffer* world_pos,
	GeometryBuffer* normal,
	GeometryBuffer* color,
	GeometryBuffer* albedo)
{
	{
		this->camera_ = *camera;

		this->cb_[0]->map(&this->camera_, 1);
		this->cb_[1]->map(&this->light_, 1);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//モデル
	auto srv_handle = descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	srv_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * 2;
	device_->CreateShaderResourceView(ort->getResource(), &srv_desc, srv_handle);

	//ワールド座標
	srv_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	device_->CreateShaderResourceView(world_pos->getOffScreenRenderTarget()->getResource(), &srv_desc, srv_handle);

	//法線
	srv_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	device_->CreateShaderResourceView(normal->getOffScreenRenderTarget()->getResource(), &srv_desc, srv_handle);

	//カラー
	srv_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	device_->CreateShaderResourceView(color->getOffScreenRenderTarget()->getResource(), &srv_desc, srv_handle);

	//アルベド
	srv_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	device_->CreateShaderResourceView(albedo->getOffScreenRenderTarget()->getResource(), &srv_desc, srv_handle);
}

void PhongShading::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_.get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	rc->setVertexBuffer(this->vb_.get());

	rc->setDescriptorHeap(this->descriptor_heap_.get());
	//model行列等
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	for (int i = 0; i < 7; i++) {
		rc->setGraphicsRootDescriptorTable(i, gpu_handle);
		gpu_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	}
	rc->drawInstanced(4);

	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

void PhongShading::debugDraw()
{
	ImGui::Begin("Phong Light");

	{//光源の方向
		float ligdir[3] = { this->light_.direction.x, this->light_.direction.y, this->light_.direction.z };
		if (ImGui::DragFloat3("light direciton", ligdir)) {
			this->light_.direction.x = ligdir[0];
			this->light_.direction.y = ligdir[1];
			this->light_.direction.z = ligdir[2];
		}
	}

	{//光源の色
		float ligcolor[3] = { this->light_.color.x, this->light_.color.y, this->light_.color.z };
		if (ImGui::DragFloat3("light color", ligcolor)) {
			this->light_.color.x = ligcolor[0];
			this->light_.color.y = ligcolor[1];
			this->light_.color.z = ligcolor[2];
		}
	}

	ImGui::End();
}

void PhongShading::loadShader()
{
	{//頂点
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "VSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		conf.file_path = "./src/shaders/PhongVS.hlsl";
		this->vs_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
	{//ピクセル
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "PSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		conf.file_path = "./src/shaders/PhongPS.hlsl";
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
	rootSignatureConf.num_srv = 5;
	rootSignatureConf.num_sampler = 1;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_PIXEL;
	rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void PhongShading::initDescriptorHeap(ID3D12Device* device)
{
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 8);
}

void PhongShading::initPipelineStateObject(ID3D12Device* device)
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
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

	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	ds_desc.FrontFace = defaultStencilOp;
	ds_desc.BackFace = defaultStencilOp;

	//ラスタライザーステート
	D3D12_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID; // ソリッド表示
	//rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE; // カリングオフ
	//rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK; // 背面カリングオン
	//rasterizer_desc.CullMode = D3D12_CULL_MODE_FRONT;
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

	//初期化
	std::vector<Vertex> vertices(4);

	vertices[0] = Vertex({ -1.0f, -1.0f, 0.0f }, { 0,1 });// 左下
	vertices[1] = Vertex({ -1.0f, 1.0f, 0.0f }, { 0,0 }); // 左上
	vertices[2] = Vertex({ 1.0f, -1.0f, 0.0f }, { 1,1 }); // 右下
	vertices[3] = Vertex({ 1.0f,  1.0f, 0.0f }, { 1,0 }); // 右上

	VertexBuffer::VertexBufferConf vb_conf = {};
	vb_conf.size = static_cast<UINT>(vertices.size() * sizeof(Vertex));  // 修正点
	vb_conf.stride = sizeof(Vertex);

	this->vb_ = VertexBufferFactory::create(vb_conf, device);
	this->vb_->map(vertices.data(), static_cast<UINT>(vertices.size()));
}

void PhongShading::initConstantBuffer(ID3D12Device* device)
{
	{
		ConstantBuffer::ConstantBufferConf c = {};
		c.size = sizeof(Camera);
		c.descriptor_heap = this->descriptor_heap_.get();
		c.slot = 0;
		this->cb_[0] = ConstantBufferFactory::create(device, c);

		this->cb_[0]->map(&this->camera_, 1);
	}

	{
		ConstantBuffer::ConstantBufferConf c = {};
		c.size = sizeof(Light);
		c.descriptor_heap = this->descriptor_heap_.get();
		c.slot = 1;
		this->cb_[1] = ConstantBufferFactory::create(device, c);
		this->cb_[1]->map(&this->light_, 1);
	}
}

void PhongShading::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = 1;
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void PhongShading::initOffScreenRenderTarget(ID3D12Device* device)
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
