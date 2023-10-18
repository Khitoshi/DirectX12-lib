#include "DX12Resources.h"

#include "DeviceContextFactory.h"
#include "CommandQueueFactory.h"
#include "SwapChainFactory.h"
#include "CommandAllocatorFactory.h"
#include "GraphicsCommandListFactory.h"
#include "RenderTargetFactory.h"
#include "CompositeRenderTargetFactory.h"
#include "DepthStencilCacheManager.h"
#include "FenceFactory.h"
#include "RenderContextFactory.h"
#include "FullScreenQuadFactory.h"
#include "GraphicsConfigurator.h"
#include "DescriptorHeapManager.h"
#include "OffScreenRenderTargetCacheManager.h"
#include <stdexcept>
#include "DescriptorHeapFactory.h"


void DX12Resources::init(HWND hWnd)
{
	hWnd_ = &hWnd;

	ComPtr<IDXGIFactory4> factory = createFactory();
	this->device_context_ = DeviceContextFactory::create(factory.Get());
	initCommandQueue();
	initSwapChain(factory.Get());
	initCommandAllocator();
	initCommandList();
	initDescriptorHeap();
	initRenderTarget();
	initCompositeRenderTarget();
	initDepthStencil();
	initFence();
	initViewport();
	initScissorRect();
	initRenderContext();
	initFullScreenQuad();
	initWindowAssociation(factory.Get());

}

void DX12Resources::beginRender()
{
	this->command_allocator_->reset();
	this->render_context_->reset(this->command_allocator_->GetAllocator(), nullptr);

	if (this->is_window_size_changed_) {
		//OnSizeChanged(1920, 1280, this->is_window_size_changed_);
		toggleFullscreen();
		is_window_size_changed_ = false;
	}

	this->frame_index_ = this->swap_chain_->getCurrentBackBufferIndex();

	this->updateDSVHandle();
}

void DX12Resources::endRender()
{
	this->composite_render_target_->beginRender(this->render_context_.get(), this->current_frame_buffer_dsv_handle_);
	this->composite_render_target_->render(this->render_context_.get(), this->device_context_->getDevice());
	this->composite_render_target_->endRender(this->render_context_.get());

	this->setMainRTVHandle();

	//Fullスクリーン四角形の描画開始処理
	this->render_context_->registerResouceBarrier(this->render_target_[this->frame_index_]->begin());
	this->render_context_->simpleStart(this->current_frame_buffer_rtv_handle_, this->current_frame_buffer_dsv_handle_);

	//offscreenをテクスチャとしたフルスクリーン四角形を描画
	full_screen_quad_->draw(this->render_context_.get(), this->device_context_->getDevice(), composite_render_target_.get());

	//Mainレンダーターゲットの描画完了を待つ
	this->render_context_->registerResouceBarrier(this->render_target_[this->frame_index_]->end());

	//リソースバリアの実行
	this->render_context_->executeResourceBarriers();

	//コマンドのクローズ
	this->render_context_->close();

	//コマンドの実行
	ID3D12CommandList* cmd_list[] = { this->command_list_->GetCommandList() };
	this->command_queue_->getCommandQueue()->ExecuteCommandLists(_countof(cmd_list), cmd_list);

	//スワップチェインの更新
	this->swap_chain_->present();

	//描画完了を待つ
	waitForGPU();
}

void DX12Resources::deinit()
{
	waitForGPU();
	this->swap_chain_->getSwapChain()->SetFullscreenState(FALSE, nullptr);
}


void DX12Resources::waitForGPU()
{
	//フェンスのシグナル
	if (FAILED(this->command_queue_->getCommandQueue()->Signal(this->fence_->getFence(), this->fence_->getValue()))) {
		throw std::runtime_error("DX12Resources::waitEndOfDrawing failed to signal fence");
	}

	//フェンスの待機
	if (this->fence_->getFence()->GetCompletedValue() < this->fence_->getValue()) {
		this->fence_->getFence()->SetEventOnCompletion(this->fence_->getValue(), this->fence_->getEvent());
		WaitForSingleObject(this->fence_->getEvent(), INFINITE);
	}

	//フェンスの値を更新
	this->fence_->incrementValue();
}

//TODO:個々の処理にhttps://github.com/techlabxe/d3d12_book_2/blob/master/common/D3D12AppBase.cppのToggleFullscreenを参考にする
//void DX12Resources::OnSizeChanged(const UINT width, const UINT height, bool minimized)
void DX12Resources::onSizeChanged()
{
	/*
	waitForGPU();

	//this->swap_chain_->SetFullScreen(is_fullscreen);

	//リリース
	//ここでエラー発生!

	Descriptor::getCache()->release(Descriptor::DescriptorType::MainRenderTarget);
	Descriptor::getCache()->release(Descriptor::DescriptorType::OffScreenRenderTarget);
	Descriptor::getCache()->release(Descriptor::DescriptorType::CompositeRenderTarget);
	Descriptor::getCache()->release(Descriptor::DescriptorType::DepthStencil);

	//GraphicsConfigurator::setWindowWidth(width);
	//GraphicsConfigurator::setWindowHeight(height);
	this->swap_chain_->resizeBuffer(GraphicsConfigurator::getWindowWidth(), GraphicsConfigurator::getWindowHeight());

	initRenderTarget();
	initCompositeRenderTarget();
	initDepthStencil();

	initViewport();
	initScissorRect();

	this->render_context_->setViewport(this->viewport_);
	this->render_context_->setScissorRect(this->viewport_);
	*/
}

void DX12Resources::toggleFullscreen()
{
	/*
	waitForGPU();

	if (this->swap_chain_->isFullScreen()) {
		//fullscreen -> windowed

		//DXGI_MODE_DESC desc = {};
		//desc.Width = GraphicsConfigurator::getWindowWidth();
		//desc.Height = GraphicsConfigurator::getWindowHeight();
		//desc.Format = this->swap_chain_->getDesc().Format;
		//desc.RefreshRate.Denominator = 1;
		//desc.RefreshRate.Numerator = 60;
		//desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		//desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		//this->swap_chain_->resizeTarget(desc);

		this->swap_chain_->setFullScreen(false);
		//SetWindowLong(*this->hWnd_, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		//ShowWindow(*this->hWnd_, SW_NORMAL);
	}
	else {
		//windowed -> fullscreen
		//DXGI_MODE_DESC desc = {};
		//desc.Width = GraphicsConfigurator::getWindowWidth();
		//desc.Height = GraphicsConfigurator::getWindowHeight();
		//desc.Format = this->swap_chain_->getDesc().Format;
		//desc.RefreshRate.Denominator = 1;
		//desc.RefreshRate.Numerator = 60;
		//desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		//desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		//this->swap_chain_->resizeTarget(desc);

		this->swap_chain_->setFullScreen(true);
	}
	onSizeChanged();
	*/

}

ComPtr<IDXGIFactory4> DX12Resources::createFactory() {
	UINT dxgi_factory_flags = 0;
#ifdef _DEBUG
	//デバッグコントローラーがあれば、デバッグレイヤーがあるDXGIを作成する。
	ComPtr<ID3D12Debug> debug_controller;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
		debug_controller->EnableDebugLayer();

		//デバッグの場合デバッグフラグを立てる
		dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;

		ComPtr<ID3D12Debug3> debug3;
		debug_controller.As(&debug3);
		if (debug3) {
			debug3->SetEnableGPUBasedValidation(true);
		}

		ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> d3dDredSettings1;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDredSettings1)))) {
			// Turn on AutoBreadcrumbs and Page Fault reporting
			d3dDredSettings1->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			d3dDredSettings1->SetBreadcrumbContextEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			d3dDredSettings1->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		}

#endif
	}
	else {
		throw std::runtime_error("failed to create debug Controller");
	}

	//factory生成
	ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory)))) {
		throw std::runtime_error("failed to create dxgi factory");
	}

	return factory;
}

void DX12Resources::initCommandQueue()
{
	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC cq_desc = {};
	cq_desc.NodeMask = 0;
	cq_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cq_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	this->command_queue_ = CommandQueueFactory::create(this->device_context_->getDevice(), cq_desc);
}

void DX12Resources::initSwapChain(IDXGIFactory4* factory) {
	SwapChain::SwapChainConf sc_conf = {};
	sc_conf.hWnd = *hWnd_;
	sc_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	sc_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	sc_conf.frame_buffer_count = GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count;
	sc_conf.factory = factory;
	sc_conf.command_queue = this->command_queue_->getCommandQueue();
	this->swap_chain_ = SwapChainFactory::create(sc_conf);
}

void DX12Resources::initCommandAllocator()
{
	this->command_allocator_ = CommandAllocatorFactory::create(this->device_context_->getDevice());
}

void DX12Resources::initCommandList()
{
	this->command_list_ = GraphicsCommandListFactory::create(this->device_context_->getDevice(), this->command_allocator_->GetAllocator());
}

void DX12Resources::initDescriptorHeap()
{
	this->rtv_descriptor_heap_ = DescriptorHeapFactory::create(this->device_context_->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count);
}

void DX12Resources::initRenderTarget()
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	this->render_target_.resize(GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count);
	//TODO:リファクタリング
	for (UINT i = 0; i < GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count; i++) {
		this->render_target_[i] = RenderTargetFactory::create(this->device_context_->getDevice(), this->swap_chain_->getSwapChain(), i, handle);
		handle.ptr += this->device_context_->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}

void DX12Resources::initCompositeRenderTarget()
{
	CompositeRenderTarget::CompositeRenderTargetConf crt_conf = {};
	//crt_conf.resource_desc = this->render_target_[0]->getResource()->GetDesc();
	crt_conf.resource_desc = this->render_target_[0]->getDesc();
	crt_conf.descriptor_heap_desc = this->rtv_descriptor_heap_->getDescriptorHeap()->GetDesc();
	this->composite_render_target_ = CompositeRenderTargetFactory::create(crt_conf, this->device_context_->getDevice());
}

void DX12Resources::initDepthStencil()
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	ds_conf.frame_buffer_count = GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, this->device_context_->getDevice());
}

void DX12Resources::initFence()
{
	this->fence_ = FenceFactory::create(this->device_context_->getDevice());
}

void DX12Resources::initViewport()
{
	this->viewport_.TopLeftX = 0;
	this->viewport_.TopLeftY = 0;
	this->viewport_.Width = static_cast<FLOAT>(GraphicsConfigurator::getInstance().getConfigurationData().window_width);
	this->viewport_.Height = static_cast<FLOAT>(GraphicsConfigurator::getInstance().getConfigurationData().window_height);
	this->viewport_.MinDepth = D3D12_MIN_DEPTH;
	this->viewport_.MaxDepth = D3D12_MAX_DEPTH;
}

void DX12Resources::initScissorRect()
{
	this->scissor_rect_.left = 0;
	this->scissor_rect_.top = 0;
	this->scissor_rect_.right = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	this->scissor_rect_.bottom = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
}

void DX12Resources::initRenderContext()
{
	this->render_context_ = RenderContextFactory::create(this->command_list_->GetCommandList(), this->viewport_);
}

void DX12Resources::initFullScreenQuad()
{
	this->full_screen_quad_ = FullScreenQuadFactory::create(this->device_context_->getDevice());
}

void DX12Resources::initWindowAssociation(IDXGIFactory4* factory)
{
	if (factory->MakeWindowAssociation(*this->hWnd_, DXGI_MWA_NO_ALT_ENTER)) {
		throw std::runtime_error("failed to make window association");
	}
}

void DX12Resources::setMainRTVHandle()
{
	//レンダーターゲットのディスクリプタヒープの先頭アドレスを取得
	this->current_frame_buffer_rtv_handle_ = this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	//フレームバッファ数分ずらす
	const UINT size = this->device_context_->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * this->frame_index_;
	this->current_frame_buffer_rtv_handle_.ptr += size;
}

void DX12Resources::setOffScreenRTVHandle()
{
	this->current_frame_buffer_rtv_handle_ = this->composite_render_target_->getRTVHeap()->GetCPUDescriptorHandleForHeapStart();
}

void DX12Resources::updateDSVHandle()
{
	this->current_frame_buffer_dsv_handle_ = this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
}