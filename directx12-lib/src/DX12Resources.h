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
class RenderTarget;

class CompositeRenderTarget;
class DepthStencil;
class RenderContext;
class Fence;
class FullScreenQuad;

using namespace Microsoft::WRL;

/// <summary>
/// DirectX12�̃��\�[�X��Z�߂ď�������N���X
/// </summary>
class DX12Resources
{
public:
    DX12Resources() :
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
        full_screen_quad_()
    {}
    ~DX12Resources() { waitEndOfDrawing(); }

    //����������
    void init(const HWND hWnd, const int width, const int height);

    //�����_�����O�J�n����
    void beginRender();

    //�����_�����O�I������
    void endRender();

    //�`��̏I����ҋ@����
    void waitEndOfDrawing();

private://�����n
    //IDXGI�t�@�N�g������
    ComPtr<IDXGIFactory4> createFactory();
    void initCommandQueue();
    //�X���b�v�`�F�C������
    void initSwapChain(const HWND hWnd, const int width, const int height, const int frameBufferCount, IDXGIFactory4* factory);
    //�R�}���h�A���P�[�^����
    void initCommandAllocator();
    //�R�}���h���X�g����
    void initCommandList();
    //Main�����_�[�^�[�Q�b�g����
    void initRenderTarget();
    //�I�t�X�N���[�������_�[�^�[�Q�b�g����
    void initCompositeRenderTarget();
    //�[�x�X�e���V������
    void initDepthStencil(const int width, const int height);
    //�t�F���X����
    void initFence();
    //�r���[�|�[�g�ݒ�
    void initViewport(const int width, const int height);
    //�V�U�����O��`�ݒ�
    void initScissorRect(const int width, const int height);
    //�����_�[�R���e�L�X�g����
    void initRenderContext();
    //�t���X�N���[���l�p�`����
    void initFullScreenQuad();

    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̐ݒ萶��
    void createOffScreenRenderTargetConf();

    //�����_�[�^�[�Q�b�g�r���[�̃n���h����ݒ�
    void setMainRTVHandle();
    //�I�t�X�N���[�������_�[�^�[�Q�b�g�r���[�̃n���h����ݒ�
    void setOffScreenRTVHandle();
    //�[�x�X�e���V���r���[�̃n���h����ݒ�
    void updateDSVHandle();
    //�I�t�X�N���[�������_�[�^�[�Q�b�g�̐ݒ���X�V
    void updateOffScreenRenderTargetConf();

public://�擾�n
    //�f�o�C�X�擾
    DeviceContext* getDeviceContext() const { return device_context_.get(); }
    //�����_�[�R���e�L�X�g�擾
    RenderContext* getRenderContext() const { return render_context_.get(); }
    //�r���[�|�[�g�擾
    D3D12_VIEWPORT getViewport() const { return viewport_; }
    //���ݏ������ݒ��̃t���[���o�b�t�@�̐[�x�X�e���V���r���[�̃n���h���擾
    D3D12_CPU_DESCRIPTOR_HANDLE getCurrentFrameBufferDSVHandle() const { return current_frame_buffer_dsv_handle_; }

private:
    std::shared_ptr<DeviceContext>device_context_;                                   //�f�o�C�X
    std::shared_ptr<CommandQueue>command_queue_;                       //�R�}���h�L���[
    std::shared_ptr<SwapChain>swap_chain_;                          //�X���b�v�`�F�C��
    std::shared_ptr<CommandAllocator>command_allocator_;               //�R�}���h�A���P�[�^
    std::shared_ptr<GraphicsCommandList> command_list_;
    std::shared_ptr<RenderTarget>render_target_;                    //�����_�[�^�[�Q�b�g
    std::shared_ptr<CompositeRenderTarget>composite_render_target_; //�I�t�X�N���[�������_�[�^�[�Q�b�g
    std::shared_ptr<DepthStencil>depth_stencil_;                    //�[�x�X�e���V��
    std::shared_ptr<Fence> fence_;                                  //�t�F���X
    D3D12_VIEWPORT viewport_;                                       //�r���[�|�[�g
    D3D12_RECT scissor_rect_;                                       //�V�U�����O��`
    std::shared_ptr<RenderContext> render_context_;                 //�����_�[�R���e�L�X�g




    D3D12_CPU_DESCRIPTOR_HANDLE current_frame_buffer_rtv_handle_;	//���ݏ������ݒ��̃t���[���o�b�t�@�̃����_�[�^�[�Q�b�g�r���[�̃n���h��
    D3D12_CPU_DESCRIPTOR_HANDLE current_frame_buffer_dsv_handle_;	//���ݏ������ݒ��̃t���[���o�b�t�@�̐[�x�X�e���V���r���[�̃n���h��

    int frame_index_;                                               //�t���[���̔ԍ�
    std::shared_ptr<FullScreenQuad> full_screen_quad_;              //�t���X�N���[���l�p�`
};
