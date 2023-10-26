#pragma once

#include "d3d12.h"
#include "RenderMode.h"

#include <memory>
#include <DirectXMath.h>
#include <stdexcept>

class RootSignature;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class DepthStencil;
class OffScreenRenderTarget;
class RenderContext;

class Triangle
{
public:
	Triangle() :
		root_signature_(),
		vertex_shader_(),
		pixel_shader_(),
		pso_(),
		vertex_buffer_(),
		index_buffer_(),
		vertices_(),
		render_mode_(RenderMode::Default),
		off_screen_render_target_(),
		device_(nullptr)
	{};
	~Triangle() {};

	void init(ID3D12Device* device);
	void draw(RenderContext* rc);

private:
	void initRootSignature(ID3D12Device* device);
	void loadShader();
	void initPipelineStateObject(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initIndexBuffer(ID3D12Device* device);
	void initOffScreenRenderTarget(ID3D12Device* device);
	void initDepthStencil(ID3D12Device* device);

public:
	void setRenderMode(RenderMode mode) {
		this->render_mode_ = mode;
	};

public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};
	void setVertices(Vertex vertices[3]);

public:
	Vertex getVertices(const int index) {
		if (index < 0 || index >= 3) {
			throw std::out_of_range("index out of range");
		}
		return this->vertices_[index];
	};

private:

	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<Shader> vertex_shader_;
	std::shared_ptr<Shader> pixel_shader_;
	std::shared_ptr<PipelineStateObject> pso_[(int)RenderMode::Num];
	std::shared_ptr<VertexBuffer> vertex_buffer_;
	std::shared_ptr<IndexBuffer> index_buffer_;
	std::shared_ptr<DepthStencil> depth_stencil_;
	Vertex vertices_[3];
	RenderMode render_mode_;
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;
	ID3D12Device* device_;
};
