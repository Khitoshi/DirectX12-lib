#pragma once

#include "d3dx12.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

class RenderContext;
class DepthStencil;
class OffScreenRenderTarget;
class RootSignature;
class DescriptorHeap;
class PipelineStateObject;

using namespace Microsoft::WRL;

class ImGuiManager
{
public:
	ImGuiManager(const HWND* hWnd) :
		hWnd_(hWnd),
		descriptor_heap_(nullptr),
		off_screen_render_target_(nullptr),
		root_signature_(nullptr),
		depth_stencil_(nullptr),
		pso_(nullptr)
	{};
	~ImGuiManager() {};

	void init(ID3D12Device* device);
	void beginRender(RenderContext* rc, ID3D12Device* device);
	void endRender(RenderContext* rc, ID3D12Device* device);
	void deinit();

private:
	void createDescriptorHeap(ID3D12Device* device);
	void createOffScreenRenderTarget(ID3D12Device* device);
	void createRootSignature(ID3D12Device* device);
	void createDepthStencil(ID3D12Device* device);
	void createPipelineStateObject(ID3D12Device* device);

public:
	std::shared_ptr<DescriptorHeap> getDescriptorHeap() { return descriptor_heap_; }


private:
	const HWND* hWnd_;
	std::shared_ptr<DescriptorHeap> descriptor_heap_;
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<DepthStencil> depth_stencil_;
	std::shared_ptr<PipelineStateObject> pso_;
};
