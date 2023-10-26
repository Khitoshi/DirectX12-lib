#include "CompositeRenderTarget.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "ShaderCacheManager.h"
#include "GraphicsConfigurator.h"
#include "VertexBufferFactory.h"
#include "RenderContext.h"
#include "RenderTargetFactory.h"
#include <stdexcept>

void CompositeRenderTarget::init(ID3D12Device* device)
{
	createSRVHeap(device);
	createSRVDesc(device);
	createRTVHeap(device);
	createRenderTarget(device);

	initRootSignature(device);
	initShader(device);
	initVertexBuffer(device);
	initPipelineStateObject(device);
}

void CompositeRenderTarget::beginRender(RenderContext* rc, D3D12_CPU_DESCRIPTOR_HANDLE depthStencil_view_handle)
{

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		getResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	rc->registerResouceBarrier(barrier);
	rc->simpleStart(this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), depthStencil_view_handle);
}

void CompositeRenderTarget::render(RenderContext* rc, ID3D12Device* device)
{
	rc->setRootSignature(this->root_signature_.get());
	rc->setPipelineState(this->pso_.get());
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	rc->setVertexBuffer(this->vertex_buffer_.get());

	// SRVHeapの始点を取得
	UINT slot = 0;
	for (auto& rt : OffScreenRenderTargetCacheManager::getInstance().getRenderTargetList())
	{
		//前回とリソースとスロットが違う場合はSRVを新しく作成する
		auto it = this->handle_chache_.find(slot);
		if (it == this->handle_chache_.end() || it->second != rt->getResource()) {
			D3D12_CPU_DESCRIPTOR_HANDLE handle = this->cbv_srv_uav_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += static_cast<unsigned long long>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * slot;

			device->CreateShaderResourceView(rt->getResource(), &srv_desc_, handle);
			this->handle_chache_[slot] = rt->getResource();
		}
		slot++;
	}
	//ディスクリプタヒープを設定
	rc->setDescriptorHeap(this->cbv_srv_uav_descriptor_heap_.get());

	//GPUハンドルをcommandlistに設定
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = this->cbv_srv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	rc->setGraphicsRootDescriptorTable(0, gpu_handle);

	gpu_handle.ptr += static_cast<unsigned long long>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * 1;
	rc->setGraphicsRootDescriptorTable(1, gpu_handle);

	rc->drawInstanced(4);
}

void CompositeRenderTarget::endRender(RenderContext* rc)
{
	OffScreenRenderTargetCacheManager::getInstance().clearRenderTargetList();
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		getResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	rc->registerResouceBarrier(barrier);
}


void CompositeRenderTarget::createSRVHeap(ID3D12Device* device)
{
	this->cbv_srv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
}

void CompositeRenderTarget::createSRVDesc(ID3D12Device* device)
{
	////シェーダーリソースビューの設定
	srv_desc_ = {};
	srv_desc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc_.Texture2D.MipLevels = 1;
	srv_desc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

void CompositeRenderTarget::createRTVHeap(ID3D12Device* device)
{
	this->rtv_descriptor_heap_ = DescriptorHeapFactory::create(device, this->conf_.descriptor_heap_desc.Type, 1);
}

void CompositeRenderTarget::createRenderTarget(ID3D12Device* device)
{
	//レンダリングターゲットビューの作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	desc.Width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, GraphicsConfigurator::getInstance().getConfigurationData().background_color);

	createCommittedResource(device, prop, D3D12_HEAP_FLAG_NONE, desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clear_value);
	device->CreateRenderTargetView(getResource(), nullptr, this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	setName(L"Composite Render Target");

}

void CompositeRenderTarget::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rs_conf = {};
	rs_conf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rs_conf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rs_conf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rs_conf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rs_conf.num_sampler = 1;
	rs_conf.num_srv = 2;
	rs_conf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rs_conf);
}

void CompositeRenderTarget::initShader(ID3D12Device* device)
{
	{//頂点シェーダーロード
		Shader::ShaderConf conf = {};
		conf.file_path = "./src/shaders/MultiPathCompositingVS.hlsl";
		conf.entry_func_name = "VSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}

	{//ピクセルシェーダーロード
		Shader::ShaderConf conf = {};
		conf.file_path = "./src/shaders/MultiPathCompositingPS.hlsl";
		conf.entry_func_name = "PSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}

void CompositeRenderTarget::initPipelineStateObject(ID3D12Device* device)
{
	//インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC input_rlement_desc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
	};

	//深度ステンシルステート
	D3D12_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = FALSE; // 深度テストを無効にする
	ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;   // 深度値の書き込みを無効にする
	ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;       // 深度比較関数を常に真にする
	ds_desc.StencilEnable = FALSE;
	ds_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	ds_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	ds_desc.FrontFace = defaultStencilOp;
	ds_desc.BackFace = defaultStencilOp;

	//パイプラインステートオブジェクトの設定
	PipelineStateObject::PipelineStateObjectConf pso_conf = {};
	pso_conf.desc.pRootSignature = this->root_signature_->getRootSignature();
	pso_conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
	pso_conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
	pso_conf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_conf.desc.SampleMask = UINT_MAX;
	pso_conf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_conf.desc.DepthStencilState = ds_desc;
	pso_conf.desc.InputLayout = { input_rlement_desc, _countof(input_rlement_desc) };
	pso_conf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso_conf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso_conf.desc.NumRenderTargets = 1;
	pso_conf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_conf.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pso_conf.desc.SampleDesc.Count = 1;
	pso_conf.desc.SampleDesc.Quality = 0;
	pso_conf.desc.NodeMask = 1;
	pso_conf.desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	this->pso_ = PSOCacheManager::getInstance().getOrCreate(device, pso_conf);
}

void CompositeRenderTarget::initVertexBuffer(ID3D12Device* device)
{
	std::vector<Vertex> vertices(4);

	vertices[0] = Vertex({ -1.0f, -1.0f, 0.0f }, { 0,1 });// 左下
	vertices[1] = Vertex({ -1.0f, 1.0f, 0.0f }, { 0,0 }); // 左上
	vertices[2] = Vertex({ 1.0f, -1.0f, 0.0f }, { 1,1 }); // 右下
	vertices[3] = Vertex({ 1.0f,  1.0f, 0.0f }, { 1,0 }); // 右上

	VertexBuffer::VertexBufferConf vb_conf = {};
	vb_conf.size = static_cast<UINT>(vertices.size() * sizeof(Vertex));
	vb_conf.stride = sizeof(Vertex);

	this->vertex_buffer_ = VertexBufferFactory::create(vb_conf, device);
	this->vertex_buffer_->map(vertices.data(), static_cast<UINT>(vertices.size()));
}

ID3D12DescriptorHeap* CompositeRenderTarget::getRTVHeap() const
{
	return rtv_descriptor_heap_->getDescriptorHeap();
}
