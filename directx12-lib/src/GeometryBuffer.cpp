#include "GeometryBuffer.h"
#include "RenderContext.h"

#include "GraphicsConfigurator.h"
#include "ShaderCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "VertexBufferFactory.h"
#include "IndexBufferFactory.h"

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
	this->initVertexBuffer(device);
	this->initIndexBuffer(device);
	this->initSRVHeap(device);
	this->initDepthStencil(device);
	this->initOffScreenRenderTarget(device);

	this->device_ = device;
}

void GeometryBuffer::update()
{

}

void GeometryBuffer::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_.get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rc->setVertexBuffer(this->vb_.get());
	rc->setIndexBuffer(this->ib_.get());
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

	//this->srv_handle_ = this->descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	this->srv_handle_ = igm->getDescriptorHeap()->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	this->srv_handle_.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * (1 + i);
	device_->CreateShaderResourceView(this->off_screen_render_target_->getResource(), &srv_desc_, this->srv_handle_);

	ImGui::Begin("G-buffer");


	//rc->setRootSignature(this->root_signature_.get());
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = igm->getDescriptorHeap()->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	gpu_handle.ptr += static_cast<unsigned long long>(this->device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * (1 + i);
	//rc->setGraphicsRootDescriptorTable(0, gpu_handle);

	ImGui::Image((void*)(intptr_t)gpu_handle.ptr, ImVec2(256, 256)); // 256x256は表示サイズ
	ImGui::End();

	//this->test_off_screen_render_target_->endRender(rc);
}

void GeometryBuffer::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rootSignatureConf = {};
	rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureConf.num_srv = 1;
	rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
	rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void GeometryBuffer::loadShader()
{
	const std::string entry_func_name = this->conf_.getSemanticName();// +this->conf_.getFormatName();
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
	//TODO:汚いのでリファクタリングする
	//if swaitchを使用しない
	std::string semantic = "";
	switch (this->conf_.semantic)
	{
	case Conf::Semantic::POSITION:
		semantic = "POSITION";
		break;

	case Conf::Semantic::NORMAL:
		semantic = "NORMAL";
		break;

	case Conf::Semantic::TEXCOORD:
		semantic = "TEXCOORD";
		break;

	case Conf::Semantic::TANGENT:
		semantic = "TANGENT";
		break;

	case Conf::Semantic::COLOR:
		semantic = "COLOR";
		break;

	}

	DXGI_FORMAT format = {};
	switch (this->conf_.format)
	{
	case Conf::Format::FLOAT4:
		format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case Conf::Format::FLOAT3:
		format = DXGI_FORMAT_R32G32B32_FLOAT;
		break;
	case Conf::Format::FLOAT2:
		format = DXGI_FORMAT_R32G32_FLOAT;
		break;
	}


	std::vector<D3D12_INPUT_ELEMENT_DESC> ied;
	if (this->conf_.semantic == Conf::Semantic::POSITION) {

		ied.push_back(
			D3D12_INPUT_ELEMENT_DESC({ semantic.c_str(), 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA })
		);
	}
	else {
		ied.push_back(
			D3D12_INPUT_ELEMENT_DESC({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA })
		);

		ied.push_back(
			D3D12_INPUT_ELEMENT_DESC({ semantic.c_str(), 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA })
		);

	}



	// インプットレイアウト
	PipelineStateObject::PipelineStateObjectConf conf = {};
	conf.desc.pRootSignature = this->root_signature_->getRootSignature();
	conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vs_->getShaderBlob());
	conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->ps_->getShaderBlob());
	conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	conf.desc.SampleMask = UINT_MAX;
	conf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	conf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	conf.desc.InputLayout = { ied.data(), static_cast<UINT>(ied.size()) };
	conf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	conf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	conf.desc.NumRenderTargets = 5;
	conf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	conf.desc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	conf.desc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
	conf.desc.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;
	conf.desc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;
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
		std::vector<DirectX::XMFLOAT4> pos(this->conf_.vertices.size());
		for (int i = 0; i < this->conf_.vertices.size(); i++) {
			pos[i] = this->conf_.vertices[i].position;
		}


		VertexBuffer::VertexBufferConf vb_conf = {};
		//vb_conf.size = sizeof(this->conf_.vertices.data());
		vb_conf.size = sizeof(DirectX::XMFLOAT4) * pos.size();;
		//vb_conf.stride = this->conf_.getStrideSize();
		vb_conf.stride = sizeof(DirectX::XMFLOAT4);
		this->vb_ = VertexBufferFactory::create(vb_conf, device);
		//this->vb_->map(this->conf_.vertices.data(), static_cast<UINT>(this->conf_.vertices.size()));
		this->vb_->map(pos.data(), static_cast<UINT>(pos.size()));

	}
	else {
		VertexBuffer::VertexBufferConf vb_conf = {};
		vb_conf.size = sizeof(DirectX::XMFLOAT4) * 2 * this->conf_.vertices.size();
		//vb_conf.stride = this->conf_.getStrideSize();
		vb_conf.stride = sizeof(DirectX::XMFLOAT4) * 2;
		this->vb_ = VertexBufferFactory::create(vb_conf, device);
		this->vb_->map(this->conf_.vertices.data(), static_cast<UINT>(this->conf_.vertices.size()));
	}

}

void GeometryBuffer::initIndexBuffer(ID3D12Device* device)
{
	//インデックスバッファの設定
	//const int numIndices = sizeof(indices) / sizeof(unsigned short);
	this->num_indices_ = static_cast<UINT>(this->conf_.indices.size());
	IndexBuffer::IndexBufferConf indexBufferConf = {};
	indexBufferConf.size = sizeof(unsigned short) * this->num_indices_;
	indexBufferConf.stride = sizeof(unsigned short);
	indexBufferConf.count = this->num_indices_;

	this->ib_ = IndexBufferFactory::create(indexBufferConf, device);
	this->ib_->map(this->conf_.indices.data(), this->num_indices_);
}

void GeometryBuffer::initSRVHeap(ID3D12Device* device)
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

void GeometryBuffer::initSRV()
{
	//D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_ = {};
	//srv_desc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srv_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//srv_desc_.Texture2D.MipLevels = 1;
	//srv_desc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//this->srv_handle_ = this->descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	//device_->CreateShaderResourceView(this->off_screen_render_target_->getResource(), &srv_desc_, this->srv_handle_);
	//device_->CreateShaderResourceView(this->off_screen_render_target_->getResource(), nullptr, this->srv_handle_);
}


