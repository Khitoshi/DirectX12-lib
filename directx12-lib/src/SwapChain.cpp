#include "SwapChain.h"
#include <stdexcept>

/// <summary>
/// ������
/// </summary>
void SwapChain::init()
{
    createSwapChain();
    createCurrentBackBufferIndex();
}

/// <summary>
/// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�����ւ���
/// </summary>
void SwapChain::present()
{
    //�X���b�v�`�F�C���̃v���[���g
    if (FAILED(this->swap_chain_->Present(1, 0))) {
        throw std::runtime_error("�X���b�v�`�F�C���̃v���[���g�Ɏ��s���܂���");
    }

    // �o�b�N�o�b�t�@�C���f�b�N�X�̍X�V
    this->current_back_buffer_index_ = this->swap_chain_->GetCurrentBackBufferIndex();
}

/// <summary>
/// �X���b�v�`�F�C������
/// </summary>
void SwapChain::createSwapChain()
{
    //�X���b�v�`�F�C���̐ݒ�
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = this->conf_.frame_buffer_count;
    swapChainDesc.Width = this->conf_.width;
    swapChainDesc.Height = this->conf_.height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    //�X���b�v�`�F�C��1����
    ComPtr<IDXGISwapChain1> swap_chain1;
    if (FAILED(this->conf_.factory->CreateSwapChainForHwnd(
        this->conf_.command_queue,
        this->conf_.hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swap_chain1
    ))) {
        throw std::runtime_error("�X���b�v�`�F�C��1�̐����Ɏ��s���܂���");
    }

    //IDXGISwapChain3�̃C���^�[�t�F�[�X���擾
    if (FAILED(swap_chain1->QueryInterface(IID_PPV_ARGS(&this->swap_chain_)))) {
        throw std::runtime_error("�X���b�v�`�F�C��3�̃C���^�[�t�F�[�X�̎擾�Ɏ��s���܂���");
    }
}

/// <summary>
/// ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾
/// </summary>
void SwapChain::createCurrentBackBufferIndex()
{
    this->current_back_buffer_index_ = this->swap_chain_->GetCurrentBackBufferIndex();
}