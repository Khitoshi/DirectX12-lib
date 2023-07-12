#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h"
#include <dxgi1_4.h>

#include "SwapChain.h"
#include "RenderTarget.h"
#include "OffScreenRenderTarget.h"
#include "DepthStencil.h"
#include "RenderContext.h"
#include "Fence.h"
#include "RootSignature.h"
#include "FullScreenQuad.h"
using namespace Microsoft::WRL;

/// <summary>
/// DirectX12�̃��\�[�X��Z�߂ď�������N���X
/// </summary>
class DX12Resources
{
public:
    DX12Resources() :
        device(nullptr),
        commandQueue(),
        swapChain(),
        commandAllocator(),
        commandList(),
        renderTarget(),
        offScreenRenderTarget(),
        depthStencil(),
        fence(),
        renderContext(),
        viewport(),
        scissorRect(),
        currentFrameBufferRTVHandle(),
        currentFrameBufferDSVHandle(),
        frameIndex(0),
        fullScreenQuad(),
        backGroundColor()
    {}
    ~DX12Resources() { waitEndOfDrawing(); }

    //����������
    void init(const HWND hWnd, const int width, const int height, const int frameBufferCount);

    //�����_�����O�J�n����
    void beginRender();

    //�����_�����O�I������
    void endRender();

    //�`��̏I����ҋ@����
    void waitEndOfDrawing();

private://�����n
    //IDXGI�t�@�N�g������
    ComPtr<IDXGIFactory4> createFactory();
    //�f�o�C�X����
    ComPtr<ID3D12Device5> createDevice(IDXGIFactory4* dxgiFactory);
    //�R�}���h�L���[����
    ComPtr<ID3D12CommandQueue> createCommandQueue();
    //�X���b�v�`�F�C������
    std::shared_ptr<SwapChain> createSwapChain(const HWND hWnd, const int width, const int height, const int frameBufferCount, IDXGIFactory4* factory);
    //�R�}���h�A���P�[�^����
    ComPtr<ID3D12CommandAllocator> createCommandAllocator();
    //�R�}���h���X�g����
    ComPtr<ID3D12GraphicsCommandList4> createCommandList();
    //Main�����_�[�^�[�Q�b�g����
    std::shared_ptr<RenderTarget> createRenderTarget(const int width, const int height, const UINT frameBufferCount);
    //�I�t�X�N���[�������_�[�^�[�Q�b�g����
    std::shared_ptr<OffScreenRenderTarget> createOffScreenRenderTarget();
    //�[�x�X�e���V������
    std::shared_ptr<DepthStencil> createDepthStencil(const int width, const int height, const UINT frameBufferCount);
    //�t�F���X����
    std::shared_ptr<Fence> createFence();
    //�����_�[�R���e�L�X�g����
    std::shared_ptr<RenderContext> createRenderContext();

    //�r���[�|�[�g�ݒ�
    D3D12_VIEWPORT createViewport(const int width, const int height);
    //�V�U�����O��`�ݒ�
    D3D12_RECT createScissorRect(const int width, const int height);

    //�����_�[�^�[�Q�b�g�r���[�̃n���h����ݒ�
    void setMainRTVHandle();
    void setOffScreenRTVHandle();

    //�[�x�X�e���V���r���[�̃n���h����ݒ�
    void setDSVHandle();

public://�ݒ�n
    void setBackGroundColor(const float color[4])
    {
        this->backGroundColor[0] = color[0];
        this->backGroundColor[1] = color[1];
        this->backGroundColor[2] = color[2];
        this->backGroundColor[3] = color[3];
    };
public://�擾�n
    //�f�o�C�X�擾
    ID3D12Device5* getDevice() const { return device.Get(); }
    //�����_�[�R���e�L�X�g�擾
    RenderContext* getRenderContext() const { return renderContext.get(); }
private:
    ComPtr<ID3D12Device5>device;                                    //�f�o�C�X
    ComPtr<ID3D12CommandQueue>commandQueue;                         //�R�}���h�L���[
    std::shared_ptr<SwapChain>swapChain;                            //�X���b�v�`�F�C��
    ComPtr<ID3D12CommandAllocator>commandAllocator;                 //�R�}���h�A���P�[�^
    ComPtr<ID3D12GraphicsCommandList4>commandList;                  //�R�}���h���X�g
    std::shared_ptr<RenderTarget>renderTarget;                      //�����_�[�^�[�Q�b�g
    std::shared_ptr<OffScreenRenderTarget>offScreenRenderTarget;    //�I�t�X�N���[�������_�[�^�[�Q�b�g
    std::shared_ptr<DepthStencil>depthStencil;                      //�[�x�X�e���V��

    std::shared_ptr<Fence> fence;                                   //�t�F���X
    std::shared_ptr<RenderContext> renderContext;                   //�����_�[�R���e�L�X�g

    D3D12_VIEWPORT viewport;                                        //�r���[�|�[�g
    D3D12_RECT scissorRect;                                         //�V�U�����O��`

    D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle;		//���ݏ������ݒ��̃t���[���o�b�t�@�̃����_�[�^�[�Q�b�g�r���[�̃n���h��
    D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferDSVHandle;		//���ݏ������ݒ��̃t���[���o�b�t�@�̐[�x�X�e���V���r���[�̃n���h��

    int frameIndex;                                                 //�t���[���̔ԍ�
    std::shared_ptr<FullScreenQuad> fullScreenQuad;                 //�t���X�N���[���l�p�`

    float backGroundColor[4];                                       //�w�i�F
};
