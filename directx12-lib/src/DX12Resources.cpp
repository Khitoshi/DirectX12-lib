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
#include "CommonGraphicsConfig.h"
#include <stdexcept>

/// <summary>
/// ����������
/// </summary>
/// <param name="hWnd">window�n���h��</param>
/// <param name="width">��</param>
/// <param name="height">����</param>
void DX12Resources::init(const HWND hWnd)
{
    //���L�����ς��̂ŉ���͍��v��1��ɂȂ�
    ComPtr<IDXGIFactory4> factory = createFactory();
    this->device_context_ = DeviceContextFactory::create(factory.Get());
    initCommandQueue();
    initSwapChain(hWnd, factory.Get());
    initCommandAllocator();
    initCommandList();
    initRenderTarget();
    initCompositeRenderTarget();
    initDepthStencil();
    initFence();
    initViewport();
    initScissorRect();
    initRenderContext();
    initFullScreenQuad();
}

/// <summary>
/// �����_�����O�J�n����
/// </summary>
/// <param name="color">clear color</param>
void DX12Resources::beginRender()
{
    //�o�b�N�o�b�t�@�C���f�b�N�X�̎擾
    this->frame_index_ = this->swap_chain_->getCurrentBackBufferIndex();

    //�R�}���h�A���P�[�^�̃��Z�b�g
    this->command_allocator_->reset();

    //�R�}���h���X�g�̃��Z�b�g
    this->render_context_->reset(this->command_allocator_->GetAllocator(), nullptr);

    //�[�x�X�e���V���r���[�̃n���h����ݒ�
    this->updateDSVHandle();
}

/// <summary>
/// �����_�����O�I������
/// </summary>
void DX12Resources::endRender()
{
    //offscreen��1���̃e�N�X�`���ɂ܂Ƃ߂�`��J�n����
    this->composite_render_target_->beginRender(this->render_context_.get(), this->current_frame_buffer_dsv_handle_);
    //offscreen��1���̃e�N�X�`���ɂ܂Ƃ߂�`�揈��
    this->composite_render_target_->render(this->render_context_.get(), this->device_context_->getDevice());
    //offscreen��1���̃e�N�X�`���ɂ܂Ƃ߂�`��I������
    this->composite_render_target_->endRender(this->render_context_.get());

    //Main�����_�[�^�[�Q�b�g�̕`��J�n

    //�����_�[�^�[�Q�b�g�̐ݒ�
    this->setMainRTVHandle();

    //Full�X�N���[���l�p�`�̕`��J�n����
    this->render_context_->transitionMainRenderTargetBegin(this->render_target_->getResource(this->frame_index_));
    this->render_context_->simpleStart(this->current_frame_buffer_rtv_handle_, this->current_frame_buffer_dsv_handle_);

    //offscreen���e�N�X�`���Ƃ����t���X�N���[���l�p�`��`��
    full_screen_quad_->draw(this->render_context_.get(), this->device_context_->getDevice(), composite_render_target_.get());

    //Main�����_�[�^�[�Q�b�g�̕`�抮����҂�
    this->render_context_->transitionMainRenderTargetEnd(this->render_target_->getResource(this->frame_index_));

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
    waitEndOfDrawing();
}

/// <summary>
/// �`��̏I����҂�
/// </summary>
void DX12Resources::waitEndOfDrawing()
{
    //�t�F���X�̃V�O�i��
    this->command_queue_->getCommandQueue()->Signal(this->fence_->getFence(), this->fence_->getValue());

    //�t�F���X�̑ҋ@
    if (this->fence_->getFence()->GetCompletedValue() < this->fence_->getValue()) {
        this->fence_->getFence()->SetEventOnCompletion(this->fence_->getValue(), this->fence_->getEvent());
        WaitForSingleObject(this->fence_->getEvent(), INFINITE);
    }

    //�t�F���X�̒l���X�V
    this->fence_->incrementValue();
}

/// <summary>
/// �t�@�N�g���@����
/// </summary>
/// <returns>
/// ���������t�@�N�g���̃|�C���g
/// </returns>
ComPtr<IDXGIFactory4> DX12Resources::createFactory() {
    UINT dxgi_factory_flags = 0;
#ifdef _DEBUG
    //�f�o�b�O�R���g���[���[������΁A�f�o�b�O���C���[������DXGI���쐬����B
    ComPtr<ID3D12Debug> debug_controller;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
        debug_controller->EnableDebugLayer();

        //�f�o�b�O�̏ꍇ�f�o�b�O�t���O�𗧂Ă�
        dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;

#if 0 // GBV ��L��������ꍇ.
        ComPtr<ID3D12Debug3> debug3;
        debug.As(&debug3);
        if (debug3) {
            debug3->SetEnableGPUBasedValidation(true);
        }

#endif
    }
    else {
        throw std::runtime_error("failed to create debug Controller");
    }

#endif
    //factory����
    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("failed to create dxgi factory");
    }

    return factory;
}

/// <summary>
/// �R�}���h�L���[�̐���
/// </summary>
void DX12Resources::initCommandQueue()
{
    //�R�}���h�L���[�̐ݒ�
    D3D12_COMMAND_QUEUE_DESC cq_desc = {};
    cq_desc.NodeMask = 0;
    cq_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cq_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    this->command_queue_ = CommandQueueFactory::create(this->device_context_->getDevice(), cq_desc);
}

/// <summary>
/// �X���b�v�`�F�C������
/// </summary>
/// <param name="hWnd">�E�B���h�E�n���h��</param>
/// <param name="width">��</param>
/// <param name="height">����</param>
/// <param name="factory">dxgi�t�@�N�g��</param>
void DX12Resources::initSwapChain(const HWND hWnd, IDXGIFactory4* factory) {
    SwapChain::SwapChainConf sc_conf = {};
    sc_conf.hWnd = hWnd;
    sc_conf.width = windowWidth;
    sc_conf.height = windowHeight;
    sc_conf.frame_buffer_count = frameBufferCount;
    sc_conf.factory = factory;
    sc_conf.command_queue = this->command_queue_->getCommandQueue();
    this->swap_chain_ = SwapChainFactory::create(sc_conf);
}

/// <summary>
/// �R�}���h�A���P�[�^����
/// </summary>
void DX12Resources::initCommandAllocator()
{
    this->command_allocator_ = CommandAllocatorFactory::create(this->device_context_->getDevice());
}

/// <summary>
/// �R�}���h���X�g����
/// </summary>
void DX12Resources::initCommandList()
{
    this->command_list_ = GraphicsCommandListFactory::create(this->device_context_->getDevice(), this->command_allocator_->GetAllocator());
}

/// <summary>
/// �����_�[�^�[�Q�b�g����
/// </summary>
void DX12Resources::initRenderTarget()
{
    RenderTarget::RenderTargetConf rt_conf = {};
    rt_conf.frame_buffer_count = frameBufferCount;
    rt_conf.swap_chain = this->swap_chain_->getSwapChain();
    this->render_target_ = RenderTargetFactory::create(rt_conf, this->device_context_->getDevice());
}

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g����
/// </summary>
void DX12Resources::initCompositeRenderTarget()
{
    CompositeRenderTarget::CompositeRenderTargetConf crt_conf = {};
    crt_conf.resource_desc = this->render_target_->getResource(0)->GetDesc();
    crt_conf.descriptor_heap_desc = this->render_target_->getDescriptorHeap()->GetDesc();
    this->composite_render_target_ = CompositeRenderTargetFactory::create(crt_conf, this->device_context_->getDevice());
}

/// <summary>
/// �[�x�X�e���V������
/// </summary>
/// <param name="width">��</param>
/// <param name="height">����</param>
void DX12Resources::initDepthStencil()
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.width = windowWidth;
    ds_conf.height = windowHeight;
    ds_conf.frame_buffer_count = frameBufferCount;
    this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, this->device_context_->getDevice());
}

/// <summary>
/// GPU��CPU�̓����I�u�W�F�N�g(fence)����
/// </summary>
void DX12Resources::initFence()
{
    this->fence_ = FenceFactory::create(this->device_context_->getDevice());
}

/// <summary>
/// �r���[�|�[�g��ݒ肷��
/// </summary>
/// <param name="width">��</param>
/// <param name="height">����</param>
void DX12Resources::initViewport()
{
    this->viewport_.TopLeftX = 0;
    this->viewport_.TopLeftY = 0;
    this->viewport_.Width = static_cast<FLOAT>(windowWidth);
    this->viewport_.Height = static_cast<FLOAT>(windowHeight);
    this->viewport_.MinDepth = D3D12_MIN_DEPTH;
    this->viewport_.MaxDepth = D3D12_MAX_DEPTH;
}

/// <summary>
/// �V�U�����O��`��ݒ肷��
/// </summary>
/// <param name="width">��</param>
/// <param name="height">����</param>
void DX12Resources::initScissorRect()
{
    this->scissor_rect_.left = 0;
    this->scissor_rect_.top = 0;
    this->scissor_rect_.right = windowWidth;
    this->scissor_rect_.bottom = windowHeight;
}

/// <summary>
/// �����_�[�R���e�L�X�g�̐���
/// </summary>
void DX12Resources::initRenderContext()
{
    this->render_context_ = RenderContextFactory::create(this->command_list_->GetCommandList(), this->viewport_);
}

/// <summary>
/// �t���X�N���[���l�p�`�𐶐�����
/// </summary>
void DX12Resources::initFullScreenQuad()
{
    this->full_screen_quad_ = FullScreenQuadFactory::create(this->device_context_->getDevice());
}

/// <summary>
/// RTV�n���h����ݒ肷��
/// </summary>
void DX12Resources::setMainRTVHandle()
{
    //�����_�[�^�[�Q�b�g�̃f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
    this->current_frame_buffer_rtv_handle_ = this->render_target_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    //�t���[���o�b�t�@�������炷
    this->current_frame_buffer_rtv_handle_.ptr += static_cast<unsigned long long>(this->render_target_->getDescriptorHeapSize()) * this->frame_index_;
}

/// <summary>
/// �I�t�X�N���[���p��RTV�n���h����ݒ肷��
/// </summary>
void DX12Resources::setOffScreenRTVHandle()
{
    this->current_frame_buffer_rtv_handle_ = this->composite_render_target_->getRTVHeap()->GetCPUDescriptorHandleForHeapStart();
}

/// <summary>
/// DSV�n���h����ݒ肷��
/// </summary>
void DX12Resources::updateDSVHandle()
{
    this->current_frame_buffer_dsv_handle_ = this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
}