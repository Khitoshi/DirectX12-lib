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
#include "GeometryBuffer.h"
#include "GraphicsConfigurator.h"
#include <stdexcept>
#include <math.h>

#include "PhongShading.h"

void Model::init(ID3D12Device* device, const char* model_file_path)
{
	this->device_ = device;

	this->loadModel(device, model_file_path);
	this->loadShader();
	this->initDescriptorHeap(device);
	this->initVertexBuffer(device);
	this->initIndexBuffer(device);
	this->initConstantBuffer(device);
	this->initTexture(device);
	this->initRootSignature(device);
	this->initPipelineStateObject(device);

	this->initOffScreenRenderTarget(device);
	this->initDepthStencil(device);

	this->initGBuffers(device);


	{
		PhongShading::Camera camera = {};
		camera.position = this->camera_position_;

		PhongShading::Light light = {};
		light.color = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		light.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
		//light.position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		phong_shading_ = std::make_shared<PhongShading>(camera, light);
		phong_shading_->init(device);
	}

}

void Model::update()
{
	this->constant_buffer_->map(&this->conf_, 1);

	for (auto& g : this->geometry_buffer_) {
		g->update(
			GeometryBuffer::Conf::Matrix(
				this->conf_.model,
				this->conf_.view,
				this->conf_.projection
			)
		);
	}

	PhongShading::Camera camera = {};
	camera.position = this->camera_position_;

	phong_shading_->update(
		&camera,
		this->off_screen_render_target_.get(),
		this->geometry_buffer_[0].get(),
		this->geometry_buffer_[1].get(),
		this->geometry_buffer_[2].get(),
		this->geometry_buffer_[3].get()
	);
}

void Model::draw(RenderContext* rc)
{
	for (int i = 0; i < this->geometry_buffer_.size(); i++) {
		//gbufferに情報を書き込む
		this->geometry_buffer_[i]->draw(rc);
	}

	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	for (size_t i = 0; i < this->meshes_.size(); i++)
	{
		rc->setRootSignature(this->root_signature_.get());
		rc->setPipelineState(this->pso_.get());
		rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		rc->setVertexBuffer(this->vertex_buffers_[i].get());
		rc->setIndexBuffer(this->index_buffers_[i].get());

		rc->setDescriptorHeap(this->descriptor_heap_.get());

		//model行列等
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
		rc->setGraphicsRootDescriptorTable(0, gpu_handle);

		//テクスチャ
		rc->setGraphicsRootDescriptorTable(1, this->meshes_[i].material.handle_gpu);

		rc->drawIndexed(this->num_indices_[i]);
	}

	this->off_screen_render_target_->endRender(rc);
	this->phong_shading_->draw(rc);
}

void Model::debugDraw(RenderContext* rc, ImGuiManager* igm)
{
	if (geometry_buffer_.empty())return;

	int i = 0;
	for (auto& g : geometry_buffer_) {
		g->debugDraw(rc, igm, i);
		i++;
	}
	phong_shading_->debugDraw();

}

void Model::initRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rootSignatureConf = {};
	rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.num_cbv = num_cb_descriptors_;
	rootSignatureConf.num_srv = num_srv_descriptors_;
	rootSignatureConf.num_sampler = 1;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
	rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void Model::initDescriptorHeap(ID3D12Device* device)
{
	num_cb_descriptors_ = 1;
	//テクスチャの種類でメッシュの数が決まっているので
	num_srv_descriptors_ = meshes_.size();

	const UINT num_descriptors = num_cb_descriptors_ + num_srv_descriptors_;
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, num_descriptors);
}

void Model::loadModel(ID3D12Device* device, const char* model_file_path)
{
	this->meshes_ = ModelMeshCacheManager::getInstance().getMeshes(model_file_path, false, true);
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
		{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
		{"TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA  },
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
	//conf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
		DirectX::XMFLOAT3 tangent;
	};

	//初期化
	this->vertex_buffers_.clear();
	this->vertex_buffers_.resize(this->meshes_.size());
	for (size_t i = 0; i < this->meshes_.size(); i++) {
		//頂点バッファの設定
		auto& vertices = this->meshes_[i].vertices;
		std::vector<Vertex> v(vertices.size());
		for (int i = 0; i < v.size(); i++) {
			v[i] = (Vertex(vertices[i].Position, vertices[i].UV, vertices[i].Tangent));
			//v.push_back(Vertex(a.Position, a.UV, a.Tangent));
		}

		VertexBuffer::VertexBufferConf conf = {};
		conf.size = static_cast<UINT>(sizeof(Vertex) * v.size());
		conf.stride = sizeof(Vertex);
		auto vb = VertexBufferFactory::create(conf, device);
		vb->map(v.data(), v.size());

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
	ConstantBuffer::ConstantBufferConf conf = {};
	conf.size = sizeof(ModelConf);
	conf.descriptor_heap = this->descriptor_heap_.get();
	conf.slot = 0;
	this->constant_buffer_ = ConstantBufferFactory::create(device, conf);

	this->constant_buffer_->map(&this->conf_, 1);
}

void Model::initTexture(ID3D12Device* device)
{
	for (size_t i = 0; i < meshes_.size(); i++) {
		auto tex = TextureCacheManager::getInstance().getOrCreate(device, meshes_[i].material.diffuse_map_name.c_str());

		const UINT slot = this->num_cb_descriptors_ + i;
		tex->createSRV(device, this->descriptor_heap_.get(), static_cast<UINT>(slot));
		auto handle = this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * static_cast<unsigned long>(slot);
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

void Model::initGBuffers(ID3D12Device* device)
{
	//メモ: meshの要素数 * メッシュのメンバ数
	this->geometry_buffer_.clear();
	for (auto& mesh : this->meshes_) {
		std::vector<GeometryBuffer::Conf::Vertex> positions;
		std::vector<GeometryBuffer::Conf::Vertex> normals;
		std::vector<GeometryBuffer::Conf::Vertex> colors;
		std::vector<GeometryBuffer::Conf::Vertex> albedoes;

		for (auto& vertex : mesh.vertices) {
			GeometryBuffer::Conf::Vertex position = {};
			position.position.x = vertex.Position.x;
			position.position.y = vertex.Position.y;
			position.position.z = vertex.Position.z;
			positions.push_back(position);

			GeometryBuffer::Conf::Vertex normal = {};
			normal.data.x = vertex.Normal.x;
			normal.data.y = vertex.Normal.y;
			normal.data.z = vertex.Normal.z;
			normal.data.w = 1.0f;
			normal.position.x = vertex.Position.x;
			normal.position.y = vertex.Position.y;
			normal.position.z = vertex.Position.z;
			normals.push_back(normal);

			GeometryBuffer::Conf::Vertex color = {};
			color.data.x = vertex.Color.x;
			color.data.y = vertex.Color.y;
			color.data.z = vertex.Color.z;
			color.data.w = vertex.Color.w;
			color.position.x = vertex.Position.x;
			color.position.y = vertex.Position.y;
			color.position.z = vertex.Position.z;
			colors.push_back(color);

			GeometryBuffer::Conf::Vertex albedo = {};
			albedo.data.x = mesh.material.diffuse_color.x;
			albedo.data.y = mesh.material.diffuse_color.y;
			albedo.data.z = mesh.material.diffuse_color.z;
			albedo.data.w = mesh.material.diffuse_color.w;
			albedo.position.x = vertex.Position.x;
			albedo.position.y = vertex.Position.y;
			albedo.position.z = vertex.Position.z;
			albedoes.push_back(albedo);
		}

		{
			this->geometry_buffer_.push_back(
				std::make_shared<GeometryBuffer>(
					GeometryBuffer::Conf(
						GeometryBuffer::Conf::Semantic::POSITION,
						positions,
						mesh.indices,
						GeometryBuffer::Conf::Matrix(
							this->conf_.model,
							this->conf_.view,
							this->conf_.projection
						)
					)
				)
			);

			this->geometry_buffer_.push_back(
				std::make_shared<GeometryBuffer>(
					GeometryBuffer::Conf(
						GeometryBuffer::Conf::Semantic::NORMAL,
						normals,
						mesh.indices,
						GeometryBuffer::Conf::Matrix(
							this->conf_.model,
							this->conf_.view,
							this->conf_.projection
						)
					)
				)
			);

			this->geometry_buffer_.push_back(
				std::make_shared<GeometryBuffer>(
					GeometryBuffer::Conf(
						GeometryBuffer::Conf::Semantic::COLOR,
						colors,
						mesh.indices,
						GeometryBuffer::Conf::Matrix(
							this->conf_.model,
							this->conf_.view,
							this->conf_.projection
						)
					)
				)
			);

			this->geometry_buffer_.push_back(
				std::make_shared<GeometryBuffer>(
					GeometryBuffer::Conf(
						GeometryBuffer::Conf::Semantic::ALBEDO,
						albedoes,
						mesh.indices,
						GeometryBuffer::Conf::Matrix(
							this->conf_.model,
							this->conf_.view,
							this->conf_.projection
						)
					)
				)
			);

		}
	}

	for (auto& gbuffer : this->geometry_buffer_) {
		gbuffer->init(device);
	}
}

void Model::initDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = 1;
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void Model::setModel(const char* model_file_path)
{
	this->init(this->device_, model_file_path);
}
