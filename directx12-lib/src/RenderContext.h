#pragma once

#include "PipelineStateObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "d3dx12.h"
#include "Texture.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"
#include "GraphicsConfigurator.h"
#include "DescriptorHeap.h"
#include <dxgi1_4.h>

using namespace Microsoft::WRL;

/// <summary>
/// �����_�[�R���e�L�X�g
/// �R�}���h���X�g�֌W�̏������܂Ƃ߂��N���X
/// </summary>
class RenderContext
{
    friend class RenderContextFactory;
private:
    RenderContext(ID3D12GraphicsCommandList4* command_list, const D3D12_VIEWPORT& viewport) :
        command_list_(command_list),
        current_viewport_(viewport)
    {};

public:
    ~RenderContext() {};

    /// <summary>
    /// �V�����R�}���h���X�g���������ꂽ�Ƃ��ɌĂяo��
    /// �R�}���h���X�g�̃��Z�b�g
    /// </summary>
    /// <param name="commandAllocator">�f�o�C�X���R�}���h���X�g���쐬���鎞�̌��ɂȂ�A���P�[�^�[</param>
    /// <param name="pipelineState">nullptr�ł�ok�ȃp�C�v���C���X�e�[�g</param>
    void reset(ID3D12CommandAllocator* command_allocator, ID3D12PipelineState* pso)
    {
        if (FAILED(this->command_list_->Reset(command_allocator, pso))) {
            throw std::exception("�R�}���h���X�g�̃��Z�b�g�Ɏ��s���܂����B");
        }
    }

    /// <summary>
    /// �R�}���h���X�g�����
    /// </summary>
    void close()
    {
        if (FAILED(this->command_list_->Close())) {
            throw std::exception("�R�}���h���X�g�̃N���[�Y�Ɏ��s���܂����B");
        }
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g�̃N���A
    /// </summary>
    /// <param name="render_target_handle">�����_�[�^�[�Q�b�g�̃n���h��</param>
    /// <param name="clear_color">RGBA</param>
    void clearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE render_target_handle, const float clear_color[4])
    {
        this->command_list_->ClearRenderTargetView(render_target_handle, clear_color, 0, nullptr);
    }

    /// <summary>
    /// �[�x�X�e���V���r���[�̃N���A
    /// </summary>
    /// <param name="dsv_handle">�[�x�X�e���V���̃n���h��</param>
    /// <param name="clear_value">�N���A�l</param>
    void clearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle, float clear_value)
    {
        this->command_list_->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clear_value, 0, 0, nullptr);
    }

    /// <summary>
    /// �r���[�|�[�g�̐ݒ�
    /// 3D�����_�����O���ꂽ�V�[�����E�B���h�E��̂ǂ̕����ɕ\�������ׂ������`
    /// �܂�A3D���[���h�̂ǂ̕������ŏI�I��2D��ʂɓ��e����邩�����肵�Ă���
    /// </summary>
    /// <param name="viewport">�ݒ肷��r���[�|�[�g</param>
    void setViewport(const D3D12_VIEWPORT& viewport)
    {
        this->command_list_->RSSetViewports(1, &viewport);
        this->current_viewport_ = viewport;
    }

    /// <summary>
    /// �V�U�����O��`�̐ݒ�
    /// ���ۂɃs�N�Z���������_�����O�����2D�����_�[�^�[�Q�b�g�̈ꕔ���`
    /// </summary>
    /// <param name="viewport">�r���[�|�[�g�Ɠ����T�C�Y�̃V�U�����O��ݒu</param>
    void setScissorRect(const D3D12_VIEWPORT& viewport)
    {
        D3D12_RECT scissorRect = {};
        scissorRect.bottom = static_cast<LONG>(viewport.Height);
        scissorRect.top = 0;
        scissorRect.left = 0;
        scissorRect.right = static_cast<LONG>(viewport.Width);
        this->command_list_->RSSetScissorRects(1, &scissorRect);
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g�̐ݒ�
    /// </summary>
    /// <param name="rtvHandle">�����_�[�^�[�Q�b�g�̃n���h��</param>
    /// <param name="dsvHandle">�[�x�X�e���V���r���[�̃n���h��</param>
    void setRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle)
    {
        //�����_�[�^�[�Q�b�g�̐ݒ�
        this->command_list_->OMSetRenderTargets(1, &rtv_handle, FALSE, &dsv_handle);
    }

    /// <summary>
    /// ���[�g�p�����[�^�ɑΉ�����f�B�X�N���v�^�e�[�u����ݒ�
    /// </summary>
    /// <param name="rootParamIndex"></param>
    /// <param name="handle"></param>
    void setGraphicsRootDescriptorTable(int root_param_index, D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        this->command_list_->SetGraphicsRootDescriptorTable(root_param_index, handle);
    }

    /// <summary>
    /// �f�B�X�N���v�^�q�[�v���e�[�u���ɓo�^
    /// </summary>
    /// <param name="descriptor_heap">�f�B�X�N���v�^�q�[�v</param>
    void setGraphicsRootDescriptorTables(const std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> descriptor_heap_handle)
    {
        // �f�B�X�N���v�^�e�[�u����o�^
        UINT num = 0;
        for (auto& d : descriptor_heap_handle) {
            this->setGraphicsRootDescriptorTable(num, d);
            num++;
        }
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g(�t�����g�o�b�t�@)�̏�ԑJ�ڂ�҂�
    /// TARGET -> PRESENT
    /// </summary>
    /// <param name="resouce">��ԑJ�ڂ����郌���_�[�^�[�Q�b�g�̃��\�[�X</param>
    void transitionMainRenderTargetEnd(ID3D12Resource* resouce)
    {
        //���\�[�X�o���A�ŁA�O���t�B�b�N�X�p�C�v���C���Ń��\�[�X�̎g�p���@���ύX�����^�C�~���O�𐧌�
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        //�o���A���R�}���h���X�g�ɒǉ�
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g(�t�����g�o�b�t�@)�ւ̕`�����ݑ҂�
    /// PRESENT -> TARGET
    /// �ɑJ��
    /// </summary>
    /// <param name="resouce">��ԑJ�ڂ����郌���_�[�^�[�Q�b�g�̃��\�[�X</param>
    void transitionMainRenderTargetBegin(ID3D12Resource* resouce) {
        //���\�[�X�o���A�ŁA�O���t�B�b�N�X�p�C�v���C���Ń��\�[�X�̎g�p���@���ύX�����^�C�~���O�𐧌�
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        //�o���A���R�}���h���X�g�ɒǉ�
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// OffScreenRenderTarget�ւ̕`�����݊J�n
    /// PIXEL_SHADER_RESOURCE -> RENDER_TARGET
    /// </summary>
    /// <param name="resouce">��ԑJ�ڂ����郌���_�[�^�[�Q�b�g�̃��\�[�X</param>
    void transitionOffScreenRenderTargetBegin(ID3D12Resource* resouce)
    {
        //���\�[�X�o���A�ŁA�O���t�B�b�N�X�p�C�v���C���Ń��\�[�X�̎g�p���@���ύX�����^�C�~���O�𐧌�
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        //�o���A���R�}���h���X�g�ɒǉ�
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// OffScreenRenderTarget�ւ̕`�����ݑ҂�
    /// RENDER_TARGET -> PIXEL_SHADER_RESOURCE
    /// </summary>
    /// <param name="resouce">��ԑJ�ڂ����郌���_�[�^�[�Q�b�g�̃��\�[�X</param>
    void transitionOffScreenRenderTargetEnd(ID3D12Resource* resouce)
    {
        //���\�[�X�o���A�ŁA�O���t�B�b�N�X�p�C�v���C���Ń��\�[�X�̎g�p���@���ύX�����^�C�~���O�𐧌�
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        //�o���A���R�}���h���X�g�ɒǉ�
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// ���\�[�X�o���A�̎��s
    /// </summary>
    void executeResourceBarriers()
    {
        if (!this->barriers_.empty()) {
            this->command_list_->ResourceBarrier(static_cast<UINT>(this->barriers_.size()), this->barriers_.data());
            this->barriers_.clear();
        }
    }

    /// <summary>
    /// �p�C�v���C���X�e�[�g�o�^
    /// </summary>
    /// <param name="pso"></param>
    void setPipelineState(PipelineStateObject* pso)
    {
        this->command_list_->SetPipelineState(pso->getPipelineStateObject());
    }

    /// <summary>
    /// �v���~�e�B�u�g�|���W�[�̐ݒ�
    /// </summary>
    /// <param name="topology"></param>
    void setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
    {
        this->command_list_->IASetPrimitiveTopology(topology);
    }

    /// <summary>
    /// ���_�o�b�t�@��ݒ�
    /// </summary>
    /// <param name="vb">���_�o�b�t�@</param>
    void setVertexBuffer(VertexBuffer* vb)
    {
        this->command_list_->IASetVertexBuffers(0, 1, &vb->getVertexBufferView());
    }

    /// <summary>
    /// �C���f�b�N�X�o�b�t�@��ݒ�
    /// </summary>
    /// <param name="ib">�C���f�b�N�X�o�b�t�@</param>
    void setIndexBuffer(IndexBuffer* ib)
    {
        this->command_list_->IASetIndexBuffer(&ib->getIndexBufferView());
    }

    /// <summary>
    /// �C���f�b�N�X�t���̕`��R�[�������s
    /// </summary>
    /// <param name="indexCount">�C���f�b�N�X�̐�</param>
    void drawIndexed(UINT indexCount) {
        this->command_list_->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
    }

    /// <summary>
    /// �C���f�b�N�X�Ȃ��̕`��R�[�������s
    /// </summary>
    /// <param name="indexCount">�C���f�b�N�X�̐�</param>
    void drawInstanced(UINT index_count)
    {
        this->command_list_->DrawInstanced(index_count, 1, 0, 0);
    }

    /// <summary>
    /// ���[�g�V�O�V�O�j�`����ݒ�
    /// </summary>
    /// <param name="rootSignature">���[�g�V�O�j�`��</param>
    void setRootSignature(RootSignature* rootSignature) {
        this->command_list_->SetGraphicsRootSignature(rootSignature->getRootSignature());
    }


    /// <summary>
    /// �f�B�X�N���v�^�q�[�v��P�̓o�^
    /// </summary>
    /// <param name="descriptor_heap"></param>
    void setDescriptorHeap(DescriptorHeap* descriptor_heap)
    {
        ID3D12DescriptorHeap* heap[] = { descriptor_heap->getDescriptorHeap() };
        this->command_list_->SetDescriptorHeaps(1, heap);
    }

    /// <summary>
    /// �f�B�X�N���v�^�q�[�v�𕡐��o�^
    /// </summary>
    /// <param name="descriptor_heap"></param>
    void setDescriptorHeaps(const std::vector<DescriptorHeap*> descriptor_heap)
    {
        // ���b�v�N���X����f�B�X�N���v�^�q�[�v�ɕϊ����ēo�^
        std::vector<ID3D12DescriptorHeap*> ds = {};
        for (auto& d : descriptor_heap) {
            ds.push_back(d->getDescriptorHeap());
        }

        this->command_list_->SetDescriptorHeaps(static_cast<UINT>(ds.size()), ds.data());
    }




    /// <summary>
    /// �P���ȕ`��J�n����
    /// </summary>
    /// <param name="rtvHandle">�����_�[�^�[�Q�b�g�r���[�n���h��</param>
    /// <param name="dsvHandle">�[�x�X�e���V���r���[�n���h��</param>
    void simpleStart(D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle)
    {
        //���\�[�X�o���A�̎��s
        this->executeResourceBarriers();

        //�����_�[�^�[�Q�b�g��ݒ�
        this->setRenderTarget(rtv_handle, dsv_handle);

        //�r���[�|�[�g�ƃV�U�[��`��ݒ�
        this->setViewport(this->current_viewport_);
        this->setScissorRect(this->current_viewport_);

        //�����_�[�^�[�Q�b�g�̃N���A
        this->clearRenderTarget(rtv_handle, GraphicsConfigurator::getBackgroundColor());

        //�[�x�X�e���V���̃N���A
        this->clearDepthStencil(dsv_handle, 1.0f);
    }

public://�擾�n
    ID3D12GraphicsCommandList4* getCommandList() { return this->command_list_; }//�R�}���h���X�g�̎擾

private:

    ID3D12GraphicsCommandList4* command_list_;        //�R�}���h���X�g
    D3D12_VIEWPORT current_viewport_;				    //���݂̃r���[�|�[�g

    std::vector<CD3DX12_RESOURCE_BARRIER> barriers_; //RESOURCE_BARRIER�̃��X�g
};
