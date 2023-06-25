#pragma once

#include "PipelineStateObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "d3dx12.h"
#include <dxgi1_4.h>
#include "RootSignature.h"
using namespace Microsoft::WRL;

/// <summary>
/// �����_�[�R���e�L�X�g
/// �R�}���h���X�g�֌W�̏������܂Ƃ߂��N���X
/// </summary>
class RenderContext
{
public:
    RenderContext() :
        commandList(),
        viewport(),
        currentViewport()
    {};
    ~RenderContext() {};

    /// <summary>
    /// DX12Resouces�̃R�}���h���X�g���Q�Ƃ���
    /// ��������邱�Ƃ�renderContext�̃R�}���h�̎��s���ł���
    /// </summary>
    /// <param name="commandList">�����ς݂̃R�}���h���X�g</param>
    void init(ID3D12GraphicsCommandList4* commandList)
    {
        this->commandList = commandList;
    }

    /// <summary>
    /// �V�����R�}���h���X�g���������ꂽ�Ƃ��ɌĂяo��
    /// �R�}���h���X�g�̃��Z�b�g
    /// </summary>
    /// <param name="commandAllocator">�f�o�C�X���R�}���h���X�g���쐬���鎞�̌��ɂȂ�A���P�[�^�[</param>
    /// <param name="pipelineState">nullptr�ł�ok�ȃp�C�v���C���X�e�[�g</param>
    void reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState)
    {
        if (FAILED(this->commandList->Reset(commandAllocator, pipelineState))) {
            throw std::exception("�R�}���h���X�g�̃��Z�b�g�Ɏ��s���܂����B");
        }
    }

    /// <summary>
    /// �R�}���h���X�g�����
    /// </summary>
    void close()
    {
        if (FAILED(this->commandList->Close())) {
            throw std::exception("�R�}���h���X�g�̃N���[�Y�Ɏ��s���܂����B");
        }
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g�̃N���A
    /// </summary>
    /// <param name="renderTargetHandle">�����_�[�^�[�Q�b�g�̃n���h��</param>
    /// <param name="clearColor">RGBA</param>
    void clearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle, const float clearColor[4])
    {
        this->commandList->ClearRenderTargetView(renderTargetHandle, clearColor, 0, nullptr);
    }

    /// <summary>
    /// �[�x�X�e���V���r���[�̃N���A
    /// </summary>
    /// <param name="dsvHandle">�[�x�X�e���V���̃n���h��</param>
    /// <param name="clearValue">�N���A�l</param>
    void clearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue)
    {
        this->commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearValue, 0, 0, nullptr);
    }

    /// <summary>
    /// �r���[�|�[�g�̐ݒ�
    /// 3D�����_�����O���ꂽ�V�[�����E�B���h�E��̂ǂ̕����ɕ\�������ׂ������`
    /// �܂�A3D���[���h�̂ǂ̕������ŏI�I��2D��ʂɓ��e����邩�����肵�Ă���
    /// </summary>
    /// <param name="viewport">�ݒ肷��r���[�|�[�g</param>
    void setViewport(const D3D12_VIEWPORT& viewport)
    {
        this->commandList->RSSetViewports(1, &viewport);
        currentViewport = viewport;
    }

    /// <summary>
    /// �V�U�����O��`�̐ݒ�
    /// ���ۂɃs�N�Z���������_�����O�����2D�����_�[�^�[�Q�b�g�̈ꕔ���`
    /// </summary>
    /// <param name="viewport">�r���[�|�[�g�Ɠ����T�C�Y�̃V�U�����O��ݒu</param>
    void setScissorRect(const D3D12_VIEWPORT& viewport)
    {
        //�V�U�����O��`��ݒ�
        D3D12_RECT scissorRect = {};
        scissorRect.bottom = static_cast<LONG>(viewport.Height);
        scissorRect.top = 0;
        scissorRect.left = 0;
        scissorRect.right = static_cast<LONG>(viewport.Width);
        //SetScissorRect(scissorRect);
        this->commandList->RSSetScissorRects(1, &scissorRect);
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g�̐ݒ�
    /// </summary>
    /// <param name="rtvHandle">�����_�[�^�[�Q�b�g�̃n���h��</param>
    /// <param name="dsvHandle">�[�x�X�e���V���r���[�̃n���h��</param>
    //void setRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
    void setRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
    {
        //�����_�[�^�[�Q�b�g�̐ݒ�
        this->commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g�̏�ԑJ�ڂ�҂�
    /// TARGET -> PRESENT
    /// </summary>
    /// <param name="renderTarget">��ԑJ�ڂ����郊�\�[�X</param>
    void TransitionRenderTargetToRender(ID3D12Resource* renderTarget)
    {
        //���\�[�X�o���A�ŁA�O���t�B�b�N�X�p�C�v���C���Ń��\�[�X�̎g�p���@���ύX�����^�C�~���O�𐧌�
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTarget,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        //�o���A���R�}���h���X�g�ɒǉ�
        this->commandList->ResourceBarrier(1, &barrier);
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g�ւ̕`�����ݑ҂�
    /// PRESENT -> TARGET
    /// �ɑJ��
    /// </summary>
    /// <param name="renderTarget">��ԑJ�ڂ����郊�\�[�X</param>
    void TransitionRenderTargetToPresent(ID3D12Resource* renderTarget) {
        //���\�[�X�o���A�ŁA�O���t�B�b�N�X�p�C�v���C���Ń��\�[�X�̎g�p���@���ύX�����^�C�~���O�𐧌�
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTarget,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        //�o���A���R�}���h���X�g�ɒǉ�
        this->commandList->ResourceBarrier(1, &barrier);
    }

    /// <summary>
    /// �f�B�X�N���v�^�q�[�v�̓o�^
    /// </summary>
    /// <param name="descriptorHeap"></param>
    void setDescriptorHeap(ID3D12DescriptorHeap* descriptorHeap)
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap };
        this->commandList->SetDescriptorHeaps(1, descriptorHeaps);
    }

    /// <summary>
    /// �p�C�v���C���X�e�[�g�o�^
    /// </summary>
    /// <param name="pso"></param>
    void setPipelineState(PipelineStateObject* pso)
    {
        this->commandList->SetPipelineState(pso->getPipelineStateObject());
    }


    /// <summary>
    /// �v���~�e�B�u�g�|���W�[�̐ݒ�
    /// </summary>
    /// <param name="topology"></param>
    void setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
    {
        this->commandList->IASetPrimitiveTopology(topology);
    }


    /// <summary>
    /// ���_�o�b�t�@��ݒ�
    /// </summary>
    /// <param name="vb">���_�o�b�t�@</param>
    void setVertexBuffer(VertexBuffer* vb)
    {
        D3D12_VERTEX_BUFFER_VIEW vbView = vb->getVertexBufferView();
        this->commandList->IASetVertexBuffers(0, 1, &vbView);
    }

    /// <summary>
    /// �C���f�b�N�X�o�b�t�@��ݒ�
    /// </summary>
    /// <param name="ib">�C���f�b�N�X�o�b�t�@</param>
    void setIndexBuffer(IndexBuffer* ib)
    {
        D3D12_INDEX_BUFFER_VIEW ibView = ib->getIndexBufferView();
        this->commandList->IASetIndexBuffer(&ibView);
    }

    /// <summary>
    /// �C���f�b�N�X�t���̕`��R�[�������s
    /// </summary>
    /// <param name="indexCount">�C���f�b�N�X�̗v�f��</param>
    void drawIndexed(UINT indexCount) {
        this->commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
        //this->commandList->DrawInstanced(indexCount, 1, 0, 0);
    }

    /// <summary>
    /// ���[�g�V�O�V�O�j�`����ݒ�
    /// </summary>
    /// <param name="rootSignature">���[�g�V�O�j�`��</param>
    void setRootSignature(RootSignature* rootSignature) {
        this->commandList->SetGraphicsRootSignature(rootSignature->getRootSignature());
    }

public:
    ID3D12GraphicsCommandList4* getCommandList() { return this->commandList; }//�R�}���h���X�g�̎擾

private:
    enum { MAX_DESCRIPTOR_HEAP = 4 };	//�f�B�X�N���v�^�q�[�v�̍ő吔
    enum { MAX_CONSTANT_BUFFER = 8 };	//�萔�o�b�t�@�̍ő吔
    enum { MAX_SHADER_RESOURCE = 16 };	//�V�F�[�_�[���\�[�X�̍ő吔

    ID3D12GraphicsCommandList4* commandList;                        //�R�}���h���X�g
    D3D12_VIEWPORT viewport;                                        //���݂̃r���[�|�[�g
    //std::vector< ComPtr<ID3D12Resource> > m_scratchResourceList;	//�X�N���b�`���\�[�X�̃��X�g�B
    D3D12_VIEWPORT currentViewport;				                    //���݂̃r���[�|�[�g
};
