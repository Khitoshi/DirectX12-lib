#pragma once
#include "d3dx12.h"

#include <memory>
#include <DirectXMath.h>
#include <stdexcept>
#include <map>

class RootSignature;
class DescriptorHeap;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class Texture;
class Camera;
class RenderContext;
class Rotation;
class RotationEffect;
class OffScreenRenderTarget;
class Shader;
class DepthStencil;

using namespace Microsoft::WRL;

/// <summary>
/// ‰æ‘œ•`‰æ—p‚ÌƒNƒ‰ƒX
/// </summary>
class Sprite
{
public:
	Sprite() :
		root_signature_(nullptr),
		descriptor_heap_(nullptr),
		pixel_shader_(nullptr),
		vertex_shader_(nullptr),
		pso_(nullptr),
		vertex_buffer_(nullptr),
		index_buffer_(nullptr),
		texture_(nullptr),
		num_indices_(0),
		vertices_(),
		off_screen_render_target_(nullptr)
	{};
	~Sprite() {};

	void init(ID3D12Device* device, const char* texture_file_path);
	void draw(RenderContext* rc);

private:
	void initRootSignature(ID3D12Device* device);
	void initDescriptorHeap(ID3D12Device* device);
	void initShader();
	void initPipelineStateObject(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initIndexBuffer(ID3D12Device* device);
	void initTexture(ID3D12Device* device, const char* texture_file_path);
	void initOffScreenRenderTarget(ID3D12Device* device);
	void initDepthStencil(ID3D12Device* device);

public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	void setVertices(Vertex vertices[4]);

	void setTexture(ID3D12Device* device, const char* texture_file_path);

public://Žæ“¾Œn
	Vertex getVertices(const int index) {
		if (index < 0 || index >= 4) {
			throw std::out_of_range("index out of range");
		}
		return vertices_[index];
	};

private:
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<DescriptorHeap> descriptor_heap_;
	std::shared_ptr<Shader> pixel_shader_;
	std::shared_ptr<Shader> vertex_shader_;
	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<VertexBuffer> vertex_buffer_;
	std::shared_ptr<IndexBuffer> index_buffer_;
	std::shared_ptr<Texture> texture_;
	Vertex vertices_[4];
	unsigned int num_indices_;
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;
	std::shared_ptr< DepthStencil> depth_stencil_;
};
