#include "RenderTarget.h"
#include "DescriptorHeapFactory.h"
#include <stdexcept>

/// <summary>
/// ����������
/// </summary>
/// <param name="renderTargetConf">RTV�����p�ݒ�</param>
void RenderTarget::init(ID3D12Device* device)
{
    createDescriptorHeap(device);
    createDescriptorHeapSize(device);
    createResource(device);
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̐���
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void RenderTarget::createDescriptorHeap(ID3D12Device* device)
{
    this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, this->conf_.frame_buffer_count);
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̃T�C�Y��ݒ�
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void RenderTarget::createDescriptorHeapSize(ID3D12Device* device)
{
    this->descriptor_heap_size_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

/// <summary>
/// ���\�[�X�̐���
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void RenderTarget::createResource(ID3D12Device* device)
{
    //�����_�����O�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�q�[�v�̐擪�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE  rtv_handle = this->descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

    //�����_�����O�^�[�Q�b�g�r���[�̍쐬loop
    for (UINT n = 0; n < this->conf_.frame_buffer_count; n++) {
        ComPtr<ID3D12Resource> r = {};
        //�X���b�v�`�F�C������o�b�N�o�b�t�@���擾
        if (FAILED(this->conf_.swap_chain->GetBuffer(n, IID_PPV_ARGS(&r)))) {
            throw std::runtime_error("failed to get swap chain buffer");
        }

        //�����_�����O�^�[�Q�b�g�r���[�̍쐬
        device->CreateRenderTargetView(
            r.Get(), nullptr, rtv_handle
        );

        //���\�[�X���i�[
        this->resource_.push_back(r);

        //�n���h�������炷
        rtv_handle.ptr += this->descriptor_heap_size_;
    }
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v���擾
/// </summary>
/// <returns></returns>
ID3D12DescriptorHeap* RenderTarget::getDescriptorHeap() const
{
    return this->descriptor_heap_->getDescriptorHeap();
}
