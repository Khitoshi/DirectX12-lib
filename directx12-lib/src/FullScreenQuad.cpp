#include "FullScreenQuad.h"

#include "VertexBufferFactory.h"

#include "RenderContext.h"
#include "ShaderCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "DescriptorHeapFactory.h"
#include "PSOCacheManager.h"
#include "CompositeRenderTarget.h"
#include "RenderTarget.h"
/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">GPUデバイス</param>
void FullScreenQuad::init(ID3D12Device* device)
{
	createShader(device);
	createVertexBuffer(device);
	createRootSignature(device);
	createPipelineState(device);
	createSRVHeap(device);
	this->num_indices_ = 4;
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
/// <param name="osrt">オフスクリーンレンダーターゲット</param>
void FullScreenQuad::draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* crt)
{
	//ルートシグネチャを設定
	rc->setRootSignature(this->root_signature_.get());
	//パイプラインステートを設定
	rc->setPipelineState(this->pso_.get());
	//プリミティブのトポロジーを設定
	rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//頂点バッファを設定
	rc->setVertexBuffer(this->vertex_buffer_.get());
	//ディスクリプタ作成
	this->createSRV(device, crt);
	//SRVヒープを設定
	rc->setDescriptorHeap(this->cbv_srv_descriptor_heap_.get());
	//GPUハンドルをcommandlistに設定
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = this->cbv_srv_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	rc->setGraphicsRootDescriptorTable(0, gpuHandle);

	//ドローコール
	rc->drawInstanced(this->num_indices_);
}

/// <summary>
/// Basicシェーダーのペア生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void FullScreenQuad::createShader(ID3D12Device* device)
{
	{//頂点シェーダー
		Shader::ShaderConf conf = {};
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		conf.entry_func_name = "VSMain";
		conf.file_path = "./src/shaders/FullScreenQuadVS.hlsl";
		this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
	{//ピクセルシェーダー
		Shader::ShaderConf conf = {};
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		conf.entry_func_name = "PSMain";
		conf.file_path = "./src/shaders/FullScreenQuadPS.hlsl";
		this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}

/// <summary>
/// 頂点バッファの作成
/// </summary>
/// <param name="device">GPUデバイス</param>
void FullScreenQuad::createVertexBuffer(ID3D12Device* device)
{
	//頂点バッファの作成
	Vertex vertices[] = {
		{{ -1.0f, -1.0f, 0.0f }, {0,1}}, // 左下
		{{ -1.0f,  1.0f, 0.0f }, {0,0}}, // 左上
		{{  1.0f, -1.0f, 0.0f }, {1,1}}, // 右下
		{{  1.0f,  1.0f, 0.0f }, {1,0}}, // 右上
	};

	VertexBuffer::VertexBufferConf conf = {};
	conf.size = sizeof(vertices);
	conf.stride = sizeof(Vertex);
	this->vertex_buffer_ = VertexBufferFactory::create(conf, device);
	this->vertex_buffer_->map(vertices, 4);
}

/// <summary>
/// ルートシグニチャの作成
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void FullScreenQuad::createRootSignature(ID3D12Device* device)
{
	//ルートシグニチャの設定(レジスタを使用しないので空にする)
	RootSignature::RootSignatureConf conf = {};
	conf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	conf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	conf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	conf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	conf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	conf.num_sampler = 1;
	conf.num_srv = 1;

	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// パイプラインステートオブジェクト作成
/// </summary>
/// <param name="device">GPUデバイス</param>
void FullScreenQuad::createPipelineState(ID3D12Device* device)
{
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
	  { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,        0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
	};

	//パイプラインステートオブジェクトの設定
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

	PipelineStateObject::PipelineStateObjectConf conf;
	conf.desc = desc;

	this->pso_ = PSOCacheManager::getInstance().getOrCreate(device, conf);
}

/// <summary>
/// ディスクリプタヒープの作成
/// </summary>
/// <param name="device"></param>
void FullScreenQuad::createSRVHeap(ID3D12Device* device)
{
	this->cbv_srv_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

void FullScreenQuad::createSRV(ID3D12Device* device, CompositeRenderTarget* osrt)
{
	//SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(osrt->getResource(), &srv_desc, this->cbv_srv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}