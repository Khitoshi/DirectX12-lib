#include "Model.h"
#include "Mesh.h"
#include "ModelMeshCacheManager.h"
#include "AssimpLoader.h"
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
#include <stdexcept>
#include <math.h>

void Model::init(ID3D12Device* device, const char* model_file_path)
{
	this->device_ = device;
	this->initRootSignature(device);
	this->loadShader();
	this->initPipelineStateObject(device);
	this->loadModel(device, model_file_path);
	this->initDescriptorHeap(device);
	this->initConstantBuffer(device);
	this->initTexture(device);
	this->initVertexBuffer(device);
	this->initIndexBuffer(device);
	this->initOffScreenRenderTarget(device);
	this->initDepthStencil(device);
}

void Model::update()
{
	this->constant_buffer_->map(&this->conf_, 1);
}

void Model::draw(RenderContext* rc)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	for (size_t i = 0; i < this->meshes_.size(); i++)
	{
		rc->setRootSignature(this->root_signature_.get());
		rc->setPipelineState(this->pso_.get());
		rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		rc->setVertexBuffer(this->vertex_buffers_[i].get());
		rc->setIndexBuffer(this->index_buffers_[i].get());

		rc->setDescriptorHeap(this->srv_cbv_uav_descriptor_heap_.get());
		auto handle = this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
		rc->setGraphicsRootDescriptorTable(0, handle);//model行列等
		handle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (1 + i);
		rc->setGraphicsRootDescriptorTable(1, handle);//メッシュの色
		rc->setGraphicsRootDescriptorTable(2, this->meshes_[i].material.handle_gpu);//テクスチャ

		rc->drawIndexed(this->num_indices_[i]);
	}

	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(off_screen_render_target_.get());
}

void Model::initRootSignature(ID3D12Device* device)
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

void Model::initDescriptorHeap(ID3D12Device* device)
{
	this->srv_cbv_uav_descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, num_descriptors_);
}

void Model::loadModel(ID3D12Device* device, const char* model_file_path)
{
	//TODO:マジックナンバーを使用しているので修正する
	this->meshes_ = ModelMeshCacheManager::getInstance().getMeshes(model_file_path, false, true);
	num_descriptors_ = static_cast<UINT>(1 + (meshes_.size() * 2));
}

void Model::loadShader()
{
	{
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "VSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::VERTEX;
		conf.file_path = "./src/shaders/3dModelVS.hlsl";
		this->vertex_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
	{
		Shader::ShaderConf conf = {};
		conf.entry_func_name = "PSMain";
		conf.current_shader_model_type = Shader::ShaderConf::ShaderModelType::PIXEL;
		conf.file_path = "./src/shaders/3dModelPS.hlsl";
		this->pixel_shader_ = ShaderCacheManager::getInstance().getOrCreate(conf);
	}
}

void Model::initPipelineStateObject(ID3D12Device* device)
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
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
	conf.desc.VS = CD3DX12_SHADER_BYTECODE(this->vertex_shader_->getShaderBlob());
	conf.desc.PS = CD3DX12_SHADER_BYTECODE(this->pixel_shader_->getShaderBlob());
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

void Model::initVertexBuffer(ID3D12Device* device)
{
	//初期化
	const size_t mesh_size = this->meshes_.size();
	this->vertex_buffers_.resize(mesh_size);
	for (size_t i = 0; i < mesh_size; i++) {
		//頂点バッファの設定
		auto& vertices = this->meshes_[i].vertices;

		VertexBuffer::VertexBufferConf conf = {};
		conf.size = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		conf.stride = sizeof(Vertex);
		auto vb = VertexBufferFactory::create(conf, device);
		vb->map(vertices.data(), vertices.size());

		this->vertex_buffers_[i] = vb;
	}
}

void Model::initIndexBuffer(ID3D12Device* device)
{
	const size_t mesh_size = this->meshes_.size();
	this->index_buffers_.resize(mesh_size);
	this->num_indices_.resize(mesh_size);
	for (size_t i = 0; i < mesh_size; i++)
	{
		auto& indices = this->meshes_[i].indices;
		//インデックスバッファの設定
		this->num_indices_[i] = static_cast<UINT>(indices.size());
		IndexBuffer::IndexBufferConf indexBufferConf = {};
		//indexBufferConf.size = sizeof(indices.data()) * this->num_indices_[i];
		indexBufferConf.size = sizeof(unsigned short) * this->num_indices_[i];
		indexBufferConf.stride = sizeof(unsigned short);
		indexBufferConf.count = this->num_indices_[i];

		//初期化
		auto ib = IndexBufferFactory::create(indexBufferConf, device);
		//コピー
		//ib->copy(indices.data());
		ib->map(indices.data(), indices.size());

		this->index_buffers_[i] = ib;
	}
}

void Model::initConstantBuffer(ID3D12Device* device)
{
	{
		ConstantBuffer::ConstantBufferConf conf = {};
		conf.size = sizeof(ModelConf);
		conf.descriptor_heap = this->srv_cbv_uav_descriptor_heap_.get();
		conf.slot = 0;
		this->constant_buffer_ = ConstantBufferFactory::create(device, conf);

		this->constant_buffer_->map(&this->conf_, 1);
	}


	{
		struct MaterialConf
		{
			DirectX::XMFLOAT4 diffuse_color; // 拡散色

		};

		mesh_color_constant_buffers_.resize(this->meshes_.size());
		for (size_t i = 0; i < this->meshes_.size(); i++) {
			MaterialConf materialConf = {};
			materialConf.diffuse_color = this->meshes_[i].material.diffuse_color;

			ConstantBuffer::ConstantBufferConf conf = {};
			conf.size = sizeof(MaterialConf);
			conf.descriptor_heap = this->srv_cbv_uav_descriptor_heap_.get();
			conf.slot = static_cast<UINT>(1 + i);
			mesh_color_constant_buffers_[i] = ConstantBufferFactory::create(device, conf);

			mesh_color_constant_buffers_[i]->map(&materialConf, 1);
		}
	}

}

void Model::initTexture(ID3D12Device* device)
{
	for (size_t i = 0; i < meshes_.size(); i++) {
		auto tex = TextureCacheManager::getInstance().getOrCreate(device, meshes_[i].material.diffuse_map_name.c_str());
		//tex->CreateShaderResourceView(device, this->srv_cbv_uav_descriptor_heap_.get(), static_cast<UINT>(meshes_.size() + 1 + i));
		tex->createSRV(device, this->srv_cbv_uav_descriptor_heap_.get(), static_cast<UINT>(meshes_.size() + 1 + i));
		auto handle = this->srv_cbv_uav_descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (this->meshes_.size() + 1 + i);
		this->meshes_[i].material.handle_gpu = handle;
	}
}

void Model::initOffScreenRenderTarget(ID3D12Device* device)
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

void Model::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = 1;
	ds_conf.width = GraphicsConfigurator::getWindowWidth();
	ds_conf.height = GraphicsConfigurator::getWindowHeight();
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void Model::setModel(const char* model_file_path)
{
	this->init(this->device_, model_file_path);
}
