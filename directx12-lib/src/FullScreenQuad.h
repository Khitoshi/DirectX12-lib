#pragma once
#include "d3dx12.h"
#include <memory>
#include <DirectXMath.h>

class VertexBuffer;
class IndexBuffer;
class RootSignature;
class DescriptorHeap;
class PipelineStateObject;
class Shader;
class RenderContext;
class Texture;
class CompositeRenderTarget;


using namespace Microsoft::WRL;

/// <summary>
/// オフスクリーンレンダーターゲットをアプリケーションのサイズに描画するクラス
/// </summary>
class FullScreenQuad
{
	friend class FullScreenQuadFactory;
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

private:
	FullScreenQuad() :
		vertex_buffer_(),
		index_buffer_(),
		root_signature_(),
		pso_(),
		pixel_shader_(),
		vertex_shader_(),
		texture_(),
		num_indices_(0),
		cbv_srv_descriptor_heap_()
	{};

public:
	~FullScreenQuad() {};
	void draw(RenderContext* rc, ID3D12Device* device, CompositeRenderTarget* osrt);

private:
	void init(ID3D12Device* device);
	void createShader(ID3D12Device* device);
	void createVertexBuffer(ID3D12Device* device);
	void createRootSignature(ID3D12Device* device);
	void createPipelineState(ID3D12Device* device);
	void createSRVHeap(ID3D12Device* device);
	void createSRV(ID3D12Device* device, CompositeRenderTarget* osrt);

private:
	std::shared_ptr<VertexBuffer> vertex_buffer_;
	std::shared_ptr<IndexBuffer> index_buffer_;
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<Shader> pixel_shader_;
	std::shared_ptr<Shader> vertex_shader_;
	std::shared_ptr<Texture> texture_;
	UINT num_indices_;
	std::shared_ptr<DescriptorHeap> cbv_srv_descriptor_heap_;
};
