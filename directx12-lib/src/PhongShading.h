#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>

class RootSignature;
class DescriptorHeap;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class DepthStencil;
class Texture;
class OffScreenRenderTarget;
class RenderContext;
class GeometryBuffer;

class PhongShading
{
public:
	struct Camera
	{
		DirectX::XMFLOAT3 position;
	};

	struct Light
	{
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 direction;
	};

public:
	PhongShading(Camera c, Light l) :
		vs_(nullptr),
		ps_(nullptr),
		root_signature_(nullptr),
		descriptor_heap_(nullptr),

		pso_(nullptr),
		vb_(),
		cb_(),
		light_(l),
		camera_(c),
		depth_stencil_(nullptr),
		off_screen_render_target_(nullptr),
		device_(nullptr)
	{};
	~PhongShading() {};

	void init(ID3D12Device* device);
	void update(
		Camera* camera,
		OffScreenRenderTarget* ort,
		GeometryBuffer* world_pos,
		GeometryBuffer* normal,
		GeometryBuffer* color,
		GeometryBuffer* albedo);
	void draw(RenderContext* rc);
	void debugDraw();

private:
	void loadShader();
	void initRootSignature(ID3D12Device* device);
	void initDescriptorHeap(ID3D12Device* device);
	void initPipelineStateObject(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initConstantBuffer(ID3D12Device* device);
	void initDepthStencil(ID3D12Device* device);
	void initOffScreenRenderTarget(ID3D12Device* device);

private:
	std::shared_ptr<Shader> vs_;
	std::shared_ptr<Shader> ps_;
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<DescriptorHeap> descriptor_heap_;
	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<VertexBuffer> vb_;
	std::shared_ptr<ConstantBuffer> cb_[2];
	Light light_;
	Camera camera_;
	std::shared_ptr<DepthStencil> depth_stencil_;
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;
	ID3D12Device* device_;

};
