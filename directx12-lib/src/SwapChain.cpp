#include "SwapChain.h"
#include <stdexcept>

/// <summary>
/// ������
/// </summary>
void SwapChain::init(const SwapChainConf swapChainConf)
{
    this->swapChain = createSwapChain(swapChainConf);

    this->currentBackBufferIndex = createCurrentBackBufferIndex();
}

/// <summary>
/// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�����ւ���
/// </summary>
void SwapChain::present()
{
    //�X���b�v�`�F�C���̃v���[���g
    if (FAILED(swapChain->Present(1, 0))) {
        throw std::runtime_error("�X���b�v�`�F�C���̃v���[���g�Ɏ��s���܂���");
    }

    // �o�b�N�o�b�t�@�C���f�b�N�X�̍X�V
    this->currentBackBufferIndex = this->swapChain->GetCurrentBackBufferIndex();
}

/// <summary>
/// �X���b�v�`�F�C������
/// </summary>
/// <returns>���������X���b�v�`�F�C��</returns>
ComPtr<IDXGISwapChain3> SwapChain::createSwapChain(const SwapChainConf swapChainConf)
{
    //�X���b�v�`�F�C���̐ݒ�
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = swapChainConf.frameBufferCount;
    swapChainDesc.Width = swapChainConf.width;
    swapChainDesc.Height = swapChainConf.height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    //�t���X�N���[���ݒ�
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
    fullScreenDesc.Windowed = TRUE;

    //�X���b�v�`�F�C��1����
    ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(swapChainConf.factory->CreateSwapChainForHwnd(
        swapChainConf.commandQueue,
        swapChainConf.hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    ))) {
        throw std::runtime_error("�X���b�v�`�F�C��1�̐����Ɏ��s���܂���");
    }

    //IDXGISwapChain3�̃C���^�[�t�F�[�X���擾
    ComPtr<IDXGISwapChain3> swapChain3;
    if (FAILED(swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain3)))) {
        throw std::runtime_error("�X���b�v�`�F�C��3�̃C���^�[�t�F�[�X�̎擾�Ɏ��s���܂���");
    }

    return swapChain3;
}

/// <summary>
/// ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾
/// </summary>
/// <returns>���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X</returns>
UINT SwapChain::createCurrentBackBufferIndex()
{
    return this->swapChain->GetCurrentBackBufferIndex();
}