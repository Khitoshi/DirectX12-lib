#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <memory>

class RenderContext;
class RootSignature;
class DescriptorHeap;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Texture;
class DepthStencil;
class OffScreenRenderTarget;


class TextureCubeModel
{
public:
	struct TextureCubeModelConf
	{
		DirectX::XMFLOAT4X4  model;
		DirectX::XMFLOAT4X4  view;
		DirectX::XMFLOAT4X4  projection;
	};
public:
	TextureCubeModel(const TextureCubeModelConf& c) :
		conf_(c),
		root_signature_(),
		vertex_shader_(),
		pixel_shader_(),
		pso_(),
		vertex_buffer_(),
		index_buffer_(),
		texture_(),
		constant_buffer_(),
		vertices_(),
		num_indices_(0),
		depth_stencil_(),
		off_screen_render_target_()
	{};
	~TextureCubeModel() {};

	void init(ID3D12Device* device, const char* texture_file_path);
	void update();
	void draw(RenderContext* rc);

private:
	void loadShader();
	void initRootSignature(ID3D12Device* device);
	void initDescriptorHeap(ID3D12Device* device);
	void initPipelineStateObject(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initIndexBuffer(ID3D12Device* device);
	void initConstantBuffer(ID3D12Device* device);
	void initTexture(ID3D12Device* device, const char* texture_file_path);
	void initDepthStencil(ID3D12Device* device);
	void initOffScreenRenderTarget(ID3D12Device* device);

public:
	void setConf(const TextureCubeModelConf& c) { this->conf_ = c; }
private:
	//頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};
	TextureCubeModelConf conf_;
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<DescriptorHeap> srv_cbv_uav_descriptor_heap_;
	std::shared_ptr<Shader> vertex_shader_;
	std::shared_ptr<Shader> pixel_shader_;
	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<VertexBuffer> vertex_buffer_;
	std::shared_ptr<IndexBuffer> index_buffer_;
	std::shared_ptr<Texture> texture_;
	UINT texture_descriptor_index_;
	std::shared_ptr<ConstantBuffer> constant_buffer_;
	std::shared_ptr<DepthStencil> depth_stencil_;
	Vertex vertices_[8];
	UINT num_indices_;
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;
};
