#include "GeometryBuffer.h"
#include "RenderContext.h"

#include "GraphicsConfigurator.h"
#include "ShaderCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "IndexBufferFactory.h"
#include "ConstantBufferFactory.h"
#include "DepthStencilCacheManager.h"
#include "OffScreenRenderTargetFactory.h"

#include <algorithm>
#include <imgui/imgui.h>
#include "ImGuiManager.h"

void GeometryBuffer::init(ID3D12Device* device)
{
	this->initRootSignature(device);
	this->loadShader();
	this->initPSO(device);
	this->initDescriptorHeap(device);
	this->initVertexBuffer(device);
	this->initIndexBuffer(device);
	this->initConstantBuffer(device);
	this->initDepthStencil(device);
	this->initOffScreenRenderTarget(device);

	this->device_ = device;
}

void GeometryBuffer::update(const Conf::Matrix& m)
{
	this->cb_->map(&m, 1);
}

void GeometryBuffer::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_.get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rc->setVertexBuffer(this->vb_.get());
	rc->setIndexBuffer(this->ib_.get());
	rc->setDescriptorHeap(this->descriptor_heap_.get());
	auto handle = this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	rc->setGraphicsRootDescriptorTable(0, handle);//model行列等

	rc->drawIndexed(this->num_indices_);

	this->off_screen_render_target_->endRender(rc);
}

void GeometryBuffer::debugDraw(RenderContext* rc, ImGuiManager* igm, int i)
{
	//画像の表示にはhttps://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-directx12-usersを参考にした
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_ = {};
	srv_desc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc_.Texture2D.MipLevels = 1;
	srv_desc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	this->srv_handle_ = igm->getDescriptorHeap()->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	this->srv_handle_.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * static_cast<unsigned long long>(1 + i);
	device_->CreateShaderResourceView(this->off_screen_render_target_->getResource(), &srv_desc_, this->srv_handle_);

	ImGui::Begin("G-buffer");

	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = igm->getDescriptorHeap()->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	gpu_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * (1 + i);
	ImGui::Text(this->conf_.getSemanticName().c_str());
	ImGui::Image((void*)(intptr_t)gpu_handle.ptr, ImVec2(256, 256)); //256x256は表示サイズ

	ImGui::End();
}

void GeometryBuffer::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rootSignatureConf = {};
	rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureConf.num_cbv = 1;
	rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
	rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void GeometryBuffer::loadShader()
{
	const std::string entry_func_name = this->conf_.getSemanticName();
	{
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "VS" + entry_func_name;
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		conf.file_path = "./src/shaders/GeometryBufferVS.hlsl";
		this->vs_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
	{
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "PS" + entry_func_name;
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		conf.file_path = "./src/shaders/GeometryBufferPS.hlsl";
		this->ps_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}

void GeometryBuffer::initPSO(ID3D12Device* device)
{
	std::string semantic = "";
	switch (this->conf_.semantic)
	{
	case Conf::Semantic::NORMAL:
		semantic = "NORMAL";
		break;
	case Conf::Semantic::COLOR:
		semantic = "COLOR";
		break;

	case Conf::Semantic::ALBEDO:
		semantic = "DIFFUSE_COLOR";
		break;
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> ied;
	ied.push_back(
		{ "POSITION",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,		0,		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
	);

	if (this->conf_.semantic != Conf::Semantic::POSITION) {
		ied.push_back(
			{ semantic.c_str(),	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,		12,		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
		);
	}

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
	//conf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	conf.desc.RasterizerState = rasterizer_desc;
	//conf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	conf.desc.DepthStencilState = ds_desc;
	//conf.desc.InputLayout = { ied.data(), _countof(ied) };
	conf.desc.InputLayout = { ied.data(), static_cast<UINT>(ied.size()) };
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

void GeometryBuffer::initVertexBuffer(ID3D12Device* device)
{
	if (this->conf_.semantic == Conf::Semantic::POSITION) {
		struct vertex
		{
			DirectX::XMFLOAT3 position;
		};
		std::vector<vertex> vertices;
		for (auto& v : this->conf_.vertices) {
			vertices.push_back({ v.position });
		}

		VertexBuffer::VertexBufferConf vb_conf = {};
		vb_conf.size = sizeof(DirectX::XMFLOAT3) * vertices.size();
		vb_conf.stride = sizeof(DirectX::XMFLOAT3);
		this->vb_ = VertexBufferFactory::create(vb_conf, device);
		this->vb_->map(vertices.data(), static_cast<UINT>(vertices.size()));
	}
	else {
		VertexBuffer::VertexBufferConf vb_conf = {};
		vb_conf.size = (sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT4)) * this->conf_.vertices.size();
		vb_conf.stride = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT4);
		this->vb_ = VertexBufferFactory::create(vb_conf, device);
		this->vb_->map(this->conf_.vertices.data(), static_cast<UINT>(this->conf_.vertices.size()));
	}
}

void GeometryBuffer::initIndexBuffer(ID3D12Device* device)
{
	//インデックスバッファの設定
	this->num_indices_ = static_cast<UINT>(this->conf_.indices.size());
	IndexBuffer::IndexBufferConf indexBufferConf = {};
	indexBufferConf.size = sizeof(unsigned short) * this->num_indices_;
	indexBufferConf.stride = sizeof(unsigned short);
	indexBufferConf.count = this->num_indices_;

	this->ib_ = IndexBufferFactory::create(indexBufferConf, device);
	this->ib_->map(this->conf_.indices.data(), this->num_indices_);
}

void GeometryBuffer::initConstantBuffer(ID3D12Device* device)
{
	ConstantBuffer::ConstantBufferConf conf = {};
	conf.size = sizeof(Conf::Matrix);
	conf.descriptor_heap = this->descriptor_heap_.get();
	conf.slot = 0;
	this->cb_ = ConstantBufferFactory::create(device, conf);

	this->cb_->map(&this->conf_.matrix, 1);
}

void GeometryBuffer::initDescriptorHeap(ID3D12Device* device)
{
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

void GeometryBuffer::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = 1;
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void GeometryBuffer::initOffScreenRenderTarget(ID3D12Device* device)
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