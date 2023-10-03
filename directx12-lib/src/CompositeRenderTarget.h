#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <map>

#include "d3dx12.h"
#include "Descriptor.h"

class   RootSignature;
class   DescriptorHeap;
class   RenderContext;
class   VertexBuffer;
class   PipelineStateObject;
class   Shader;

/// <summary>
/// オフスクリーンを1枚のテクスチャに合成するクラス
/// </summary>
class CompositeRenderTarget :public Descriptor
{
	friend class CompositeRenderTargetFactory;
public:
	struct CompositeRenderTargetConf {
		D3D12_RESOURCE_DESC resource_desc;
		D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
	};

private:
	CompositeRenderTarget(CompositeRenderTargetConf c) :
		Descriptor(Descriptor::DescriptorType::CompositeRenderTarget),
		conf_(c),
		cbv_srv_uav_descriptor_heap_(),
		rtv_descriptor_heap_(),
		pso_(),
		vertex_buffer_(),
		root_signature_(),
		pixel_shader_(),
		vertex_shader_(),
		srv_desc_(),
		handle_chache_()
	{};
public:
	~CompositeRenderTarget() {};

	void beginRender(RenderContext* rc, D3D12_CPU_DESCRIPTOR_HANDLE depthStencil_view_handle);
	void render(RenderContext* rc, ID3D12Device* device);
	void endRender(RenderContext* rc);

	void init(ID3D12Device* device)override;
private:

	void createSRVHeap(ID3D12Device* device);
	void createSRVDesc(ID3D12Device* device);
	void createRTVHeap(ID3D12Device* device);
	void createRenderTarget(ID3D12Device* device);

	void initRootSignature(ID3D12Device* device);
	void initShader(ID3D12Device* device);
	void initPipelineStateObject(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);

public:
	ID3D12DescriptorHeap* getRTVHeap() const;

private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	CompositeRenderTargetConf conf_;

	std::shared_ptr<DescriptorHeap> cbv_srv_uav_descriptor_heap_;
	std::shared_ptr<DescriptorHeap> rtv_descriptor_heap_;

	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<VertexBuffer> vertex_buffer_;
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<Shader> pixel_shader_;
	std::shared_ptr<Shader> vertex_shader_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_;

	std::map<UINT, ID3D12Resource*> handle_chache_;

};
