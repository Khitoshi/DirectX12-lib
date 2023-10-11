#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>

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

class PhongShading
{
public:
	PhongShading() :
		vs_(nullptr),
		ps_(nullptr),
		root_signature_(nullptr),
		srv_cbv_uav_descriptor_heap_(nullptr),

		pso_(nullptr),
		vb_(),
		cb_(nullptr),
		depth_stencil_(nullptr),
		off_screen_render_target_(nullptr)
	{};
	~PhongShading() {};

	void init(ID3D12Device* device);
	void update();
	void draw(RenderContext* rc);

private:
	void loadShader();
	void initRootSignature(ID3D12Device* device);
	void initDescriptorHeap(ID3D12Device* device);
	void initPipelineStateObject(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initConstantBuffer(ID3D12Device* device);
	void initDepthStencil(ID3D12Device* device);
	void initOffScreenRenderTarget(ID3D12Device* device);


	void initTexture(ID3D12Device* device, OffScreenRenderTarget* offscreen_rt);
private:
	struct PhongConf
	{

	};

	std::shared_ptr<Shader> vs_;
	std::shared_ptr<Shader> ps_;
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<DescriptorHeap> srv_cbv_uav_descriptor_heap_;
	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<VertexBuffer> vb_;
	PhongConf conf_;
	std::shared_ptr<ConstantBuffer> cb_;
	std::shared_ptr<DepthStencil> depth_stencil_;

	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;
};
