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

	loadGraphicsConf();
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
}

void DX12Resources::beginRender()
{
	this->frame_index_ = this->swap_chain_->getCurrentBackBufferIndex();
	this->command_allocator_->reset();
	this->render_context_->reset(this->command_allocator_->GetAllocator(), nullptr);
	this->updateDSVHandle();
}

void DX12Resources::endRender()
{
	this->composite_render_target_->beginRender(this->render_context_.get(), this->current_frame_buffer_dsv_handle_);
	this->composite_render_target_->render(this->render_context_.get(), this->device_context_->getDevice());
	this->composite_render_target_->endRender(this->render_context_.get());

	this->setMainRTVHandle();

	//Full�X�N���[���l�p�`�̕`��J�n����
	//this->render_context_->transitionMainRenderTargetBegin(this->render_target_[this->frame_index_]->getResource());
	this->render_context_->registerResouceBarrier(this->render_target_[this->frame_index_]->begin());
	this->render_context_->simpleStart(this->current_frame_buffer_rtv_handle_, this->current_frame_buffer_dsv_handle_);

	//offscreen���e�N�X�`���Ƃ����t���X�N���[���l�p�`��`��
	full_screen_quad_->draw(this->render_context_.get(), this->device_context_->getDevice(), composite_render_target_.get());

	//Main�����_�[�^�[�Q�b�g�̕`�抮����҂�
	//this->render_context_->transitionMainRenderTargetEnd(this->render_target_[this->frame_index_]->end());
	this->render_context_->registerResouceBarrier(this->render_target_[this->frame_index_]->end());

	//���\�[�X�o���A�̎��s
	this->render_context_->executeResourceBarriers();

	//�R�}���h�̃N���[�Y
	this->render_context_->close();

	//�R�}���h�̎��s
	ID3D12CommandList* cmd_list[] = { this->command_list_->GetCommandList() };
	this->command_queue_->getCommandQueue()->ExecuteCommandLists(_countof(cmd_list), cmd_list);

	//�X���b�v�`�F�C���̍X�V
	this->swap_chain_->present();

	//�`�抮����҂�
	waitForGPU();
}

void DX12Resources::deinit()
{
	waitForGPU();
	this->swap_chain_->getSwapChain()->SetFullscreenState(FALSE, nullptr);
}

/// <summary>
/// �`��̏I����҂�
/// </summary>
void DX12Resources::waitForGPU()
{
	//�t�F���X�̃V�O�i��
	if (FAILED(this->command_queue_->getCommandQueue()->Signal(this->fence_->getFence(), this->fence_->getValue()))) {
		throw std::runtime_error("DX12Resources::waitEndOfDrawing failed to signal fence");
	}

	//�t�F���X�̑ҋ@
	if (this->fence_->getFence()->GetCompletedValue() < this->fence_->getValue()) {
		this->fence_->getFence()->SetEventOnCompletion(this->fence_->getValue(), this->fence_->getEvent());
		WaitForSingleObject(this->fence_->getEvent(), INFINITE);
	}

	//�t�F���X�̒l���X�V
	this->fence_->incrementValue();
}

//void DX12Resources::OnSizeChanged(const UINT width, const UINT height, bool minimized)
void DX12Resources::OnSizeChanged()
{
	waitForGPU();

	//�����[�X
	Descriptor::getCache()->release(Descriptor::DescriptorType::MainRenderTarget);
	Descriptor::getCache()->release(Descriptor::DescriptorType::OffScreenRenderTarget);
	Descriptor::getCache()->release(Descriptor::DescriptorType::CompositeRenderTarget);
	Descriptor::getCache()->release(Descriptor::DescriptorType::DepthStencil);

	GraphicsConfigurator::setWindowHeight(1920);
	GraphicsConfigurator::setWindowWidth(1280);
	this->swap_chain_->resizeBuffer(1920, 1280);

	//Descriptor::getCache()->regenerate(Descriptor::DescriptorType::MainRenderTarget, this->device_context_->getDevice());
	initRenderTarget();
	//Descriptor::getCache()->regenerate(Descriptor::DescriptorType::OffScreenRenderTarget, this->device_context_->getDevice());
	Descriptor::getCache()->regenerate(Descriptor::DescriptorType::CompositeRenderTarget, this->device_context_->getDevice());
	//Descriptor::getCache()->regenerate(Descriptor::DescriptorType::DepthStencil, this->device_context_->getDevice());

	initViewport();
	initScissorRect();

	/*
	for (int i = 0; i < this->swap_chain_->getCurrentBackBufferIndex(); i++) {
		this->render_target_->resourceReset(i);
	}
	OffScreenRenderTargetCacheManager::getInstance().clearRenderTargetTest();

	DescriptorHeapManager::clear(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DescriptorHeapManager::clear(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// �t���X�N���[����Ԃ̐؂�ւ�
	static BOOL is_fullscreen = FALSE; // �O���[�o���܂��̓����o�ϐ��Ƃ��ĕێ�����
	is_fullscreen != is_fullscreen;
	this->swap_chain_->getSwapChain()->SetFullscreenState(!is_fullscreen, nullptr);

	// �K�v�ɉ����ăo�b�t�@�����T�C�Y
	UINT width = !is_fullscreen ? 1920 : 1280; // �T�C�Y��K�؂ɐݒ�
	UINT height = !is_fullscreen ? 1080 : 720;
	GraphicsConfigurator::setWindowHeight(width);
	GraphicsConfigurator::setWindowWidth(height);

	this->swap_chain_->resizeBuffer(width, height);


	// �V�������\�[�X�̍쐬
	DescriptorHeapManager::reinit(this->device_context_->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	OffScreenRenderTargetCacheManager::getInstance().reinit(this->device_context_->getDevice());
	DescriptorHeapManager::reinit(this->device_context_->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	initViewport();
	initScissorRect();
	*/

}

void DX12Resources::loadGraphicsConf()
{
	GraphicsConfigurator::init();
}

ComPtr<IDXGIFactory4> DX12Resources::createFactory() {
	UINT dxgi_factory_flags = 0;
#ifdef _DEBUG
	//�f�o�b�O�R���g���[���[������΁A�f�o�b�O���C���[������DXGI���쐬����B
	ComPtr<ID3D12Debug> debug_controller;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
		debug_controller->EnableDebugLayer();

		//�f�o�b�O�̏ꍇ�f�o�b�O�t���O�𗧂Ă�
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

	//factory����
	ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory)))) {
		throw std::runtime_error("failed to create dxgi factory");
	}

	return factory;
}

void DX12Resources::initCommandQueue()
{
	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC cq_desc = {};
	cq_desc.NodeMask = 0;
	cq_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cq_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	this->command_queue_ = CommandQueueFactory::create(this->device_context_->getDevice(), cq_desc);
}

void DX12Resources::initSwapChain(IDXGIFactory4* factory) {
	SwapChain::SwapChainConf sc_conf = {};
	sc_conf.hWnd = *hWnd_;
	sc_conf.width = GraphicsConfigurator::getWindowWidth();
	sc_conf.height = GraphicsConfigurator::getWindowHeight();
	sc_conf.frame_buffer_count = GraphicsConfigurator::getFrameBufferCount();
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
	this->rtv_descriptor_heap_ = DescriptorHeapFactory::create(this->device_context_->getDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, GraphicsConfigurator::getFrameBufferCount());
}

void DX12Resources::initRenderTarget()
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	this->render_target_.resize(GraphicsConfigurator::getFrameBufferCount());

	for (UINT i = 0; i < GraphicsConfigurator::getFrameBufferCount(); i++) {
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
	ds_conf.width = GraphicsConfigurator::getWindowWidth();
	ds_conf.height = GraphicsConfigurator::getWindowHeight();
	ds_conf.frame_buffer_count = GraphicsConfigurator::getFrameBufferCount();
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
	this->viewport_.Width = static_cast<FLOAT>(GraphicsConfigurator::getWindowWidth());
	this->viewport_.Height = static_cast<FLOAT>(GraphicsConfigurator::getWindowHeight());
	this->viewport_.MinDepth = D3D12_MIN_DEPTH;
	this->viewport_.MaxDepth = D3D12_MAX_DEPTH;
}

void DX12Resources::initScissorRect()
{
	this->scissor_rect_.left = 0;
	this->scissor_rect_.top = 0;
	this->scissor_rect_.right = GraphicsConfigurator::getWindowWidth();
	this->scissor_rect_.bottom = GraphicsConfigurator::getWindowHeight();
}

void DX12Resources::initRenderContext()
{
	this->render_context_ = RenderContextFactory::create(this->command_list_->GetCommandList(), this->viewport_);
}

void DX12Resources::initFullScreenQuad()
{
	this->full_screen_quad_ = FullScreenQuadFactory::create(this->device_context_->getDevice());
}

void DX12Resources::setMainRTVHandle()
{
	//�����_�[�^�[�Q�b�g�̃f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
	this->current_frame_buffer_rtv_handle_ = this->rtv_descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	//�t���[���o�b�t�@�������炷
	//this->current_frame_buffer_rtv_handle_.ptr += static_cast<unsigned long long>(this->render_target_->getDescriptorHeapSize()) * this->frame_index_;
	const UINT size = this->device_context_->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * this->frame_index_;
	//this->current_frame_buffer_rtv_handle_.ptr += static_cast<unsigned long long>(size) * this->frame_index_;
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