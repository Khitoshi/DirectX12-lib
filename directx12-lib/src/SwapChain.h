#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
using namespace Microsoft::WRL;

//�X���b�v�`�F�C���̐������Ɏg�p����ݒ�
struct SwapChainConf {
    UINT frameBufferCount;           //�o�b�t�@��
    UINT width;                      //��
    UINT height;                     //����
    HWND hWnd;                       //�E�B���h�E�n���h��
    IDXGIFactory4* factory;      //DXGI�t�@�N�g��
    ID3D12CommandQueue* commandQueue;//�R�}���h�L���[
};

/// <summary>
/// �X���b�v�`�F�C��
/// </summary>
class SwapChain
{
public:

public:
    SwapChain() :
        swapChain(),
        currentBackBufferIndex(0)
    {};
    ~SwapChain() {};

    //�X���b�v�`�F�C���֌W�̐���
    void init(const SwapChainConf swapChainConf);

    //�X���b�v�`�F�C���̃v���[���g
    void present();

private:
    //�X���b�v�`�F�C���̐���
    ComPtr<IDXGISwapChain3> createSwapChain(const SwapChainConf swapChainConf);
    //�o�b�N�o�b�t�@�̐���
    UINT createCurrentBackBufferIndex();

public:
    IDXGISwapChain3* getSwapChain() const { return swapChain.Get(); } //�X���b�v�`�F�C���̎擾
    UINT getCurrentBackBufferIndex() const { return currentBackBufferIndex; } //���݂̃o�b�N�o�b�t�@�̔ԍ��̎擾
private:
    ComPtr<IDXGISwapChain3> swapChain;      //�X���b�v�`�F�C��
    UINT currentBackBufferIndex;            //���݂̃o�b�N�o�b�t�@�̔ԍ�
};
