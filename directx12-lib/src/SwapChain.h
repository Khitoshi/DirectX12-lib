#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>

using namespace Microsoft::WRL;

/// <summary>
/// �X���b�v�`�F�C��
/// </summary>
class SwapChain
{
    friend class SwapChainFactory;
public:
    //�X���b�v�`�F�C���̐������Ɏg�p����ݒ�
    struct SwapChainConf {
        UINT frame_buffer_count;           //�o�b�t�@��
        UINT width;                      //��
        UINT height;                     //����
        HWND hWnd;                       //�E�B���h�E�n���h��
        IDXGIFactory4* factory;      //DXGI�t�@�N�g��
        ID3D12CommandQueue* command_queue;//�R�}���h�L���[
    };

private:
    SwapChain(const SwapChainConf& c) :
        conf_(c),
        swap_chain_(),
        current_back_buffer_index_(0)
    {};

public:
    ~SwapChain() {};

    //�X���b�v�`�F�C���̃v���[���g
    void present();

private:
    //�X���b�v�`�F�C���֌W�̐���
    void init();
    //�X���b�v�`�F�C���̐���
    void createSwapChain();
    //�o�b�N�o�b�t�@�̐���
    void createCurrentBackBufferIndex();

public:
    IDXGISwapChain3* getSwapChain() const { return swap_chain_.Get(); }             //�X���b�v�`�F�C���̎擾
    UINT getCurrentBackBufferIndex() const { return current_back_buffer_index_; }   //���݂̃o�b�N�o�b�t�@�̔ԍ��̎擾

private:
    SwapChainConf conf_;                    //�ݒ�
    ComPtr<IDXGISwapChain3> swap_chain_;    //�X���b�v�`�F�C��
    UINT current_back_buffer_index_;        //���݂̃o�b�N�o�b�t�@�̔ԍ�
};
