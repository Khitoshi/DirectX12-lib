#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include "d3dx12.h"
#include <dxgi1_4.h>

class DeviceContext;
class CommandQueue;
class SwapChain;
class CommandAllocator;
class GraphicsCommandList;
class DescriptorHeap;
class RenderTarget;

class CompositeRenderTarget;
class DepthStencil;
class RenderContext;
class Fence;
class FullScreenQuad;

using namespace Microsoft::WRL;

/// <summary>
/// DirectX12ÇÃÉäÉ\Å[ÉXÇìZÇﬂÇƒèàóùÇ∑ÇÈÉNÉâÉX
/// </summary>
class DX12Resources
{
public:
	DX12Resources() :
		hWnd_(nullptr),
		device_context_(nullptr),
		command_queue_(),
		swap_chain_(),
		command_allocator_(),
		command_list_(),
		render_target_(),
		composite_render_target_(),
		depth_stencil_(),
		fence_(),
		viewport_(),
		scissor_rect_(),
		render_context_(),
		current_frame_buffer_rtv_handle_(),
		current_frame_buffer_dsv_handle_(),
		frame_index_(0),
		full_screen_quad_(),
		is_window_size_changed_(false)
	{}
	~DX12Resources() {}

	void init(HWND hWnd);
	void beginRender();
	void endRender();
	void deinit();
	void waitForGPU();

	void setWindowSizeChanged(bool is_window_size_changed) { is_window_size_changed_ = is_window_size_changed; }

private://ê∂ê¨ån

	ComPtr<IDXGIFactory4> createFactory();
	void initCommandQueue();
	void initSwapChain(IDXGIFactory4* factory);
	void initCommandAllocator();
	void initCommandList();
	void initDescriptorHeap();
	void initRenderTarget();
	void initCompositeRenderTarget();
	void initDepthStencil();
	void initFence();
	void initViewport();
	void initScissorRect();
	void initRenderContext();
	void initFullScreenQuad();
	void initWindowAssociation(IDXGIFactory4* factory);

	void setMainRTVHandle();
	void setOffScreenRTVHandle();
	void updateDSVHandle();

public://éÊìæån
	DeviceContext* getDeviceContext() const { return device_context_.get(); }
	RenderContext* getRenderContext() const { return render_context_.get(); }
	D3D12_VIEWPORT getViewport() const { return viewport_; }
	D3D12_CPU_DESCRIPTOR_HANDLE getCurrentFrameBufferDSVHandle() const { return current_frame_buffer_dsv_handle_; }

private:
	HWND* hWnd_;

	std::shared_ptr<DeviceContext>device_context_;
	std::shared_ptr<CommandQueue>command_queue_;
	std::shared_ptr<SwapChain>swap_chain_;
	std::shared_ptr<CommandAllocator>command_allocator_;
	std::shared_ptr<GraphicsCommandList> command_list_;
	std::shared_ptr<DescriptorHeap> rtv_descriptor_heap_;
	std::vector<std::shared_ptr<RenderTarget>>render_target_;
	std::shared_ptr<CompositeRenderTarget>composite_render_target_;
	std::shared_ptr<DepthStencil>depth_stencil_;
	std::shared_ptr<Fence> fence_;
	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissor_rect_;
	std::shared_ptr<RenderContext> render_context_;

	D3D12_CPU_DESCRIPTOR_HANDLE current_frame_buffer_rtv_handle_;
	D3D12_CPU_DESCRIPTOR_HANDLE current_frame_buffer_dsv_handle_;

	int frame_index_;
	std::shared_ptr<FullScreenQuad> full_screen_quad_;

	bool is_window_size_changed_;

};
