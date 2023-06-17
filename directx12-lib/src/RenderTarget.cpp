#include "RenderTarget.h"
#include <stdexcept>

/// <summary>
/// ����������
/// </summary>
/// <param name="renderTargetConf"></param>
void RenderTarget::init(RenderTargetConf renderTargetConf)
{
    this->descriptorHeap = createDescriptorHeap(renderTargetConf);
    this->descriptorHeapSize = createDescriptorHeapSize(renderTargetConf);
    this->resource = createResource(renderTargetConf);
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̐���
/// </summary>
/// <param name="renderTargetConf">�����_�[�^�[�Q�b�g�����̐ݒ�</param>
/// <returns>
/// ���������f�B�X�N���v�^�q�[�v
/// </returns>
ComPtr<ID3D12DescriptorHeap> RenderTarget::createDescriptorHeap(const RenderTargetConf renderTargetConf)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    desc.NumDescriptors = renderTargetConf.frameBufferCount;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;

    ComPtr<ID3D12DescriptorHeap> dh = nullptr;

    if (FAILED(renderTargetConf.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dh)))) {
        throw std::runtime_error("failed to create render target descriptor heap");
    }

    return dh;
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v�̃T�C�Y���擾
/// </summary>
/// <param name="renderTargetConf">�����_�[�^�[�Q�b�g�����̐ݒ�</param>
/// <returns>
/// �擾�����f�B�X�N���v�^�q�[�v�̃T�C�Y
/// </returns>
int RenderTarget::createDescriptorHeapSize(RenderTargetConf renderTargetConf)
{
    return renderTargetConf.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

/// <summary>
/// ���\�[�X�̍쐬
/// </summary>
/// <param name="renderTargetConf">�����_�[�^�[�Q�b�g�����̐ݒ�</param>
/// <returns>
/// �����������\�[�X
/// </returns>
std::vector<ComPtr<ID3D12Resource>> RenderTarget::createResource(const RenderTargetConf renderTargetConf)
{
    //�����_�����O�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�q�[�v�̐擪�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE  rtvHandle = this->descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart();

    //�����_�����O�^�[�Q�b�g�r���[�̍쐬loop
    std::vector<ComPtr<ID3D12Resource>> resource;
    for (UINT n = 0; n < renderTargetConf.frameBufferCount; n++) {
        ComPtr<ID3D12Resource> r = nullptr;
        //�X���b�v�`�F�C������o�b�N�o�b�t�@���擾
        if (FAILED(renderTargetConf.swapChain->GetBuffer(n, IID_PPV_ARGS(&r)))) {
            throw std::runtime_error("failed to get swap chain buffer");
        }

        //�����_�����O�^�[�Q�b�g�r���[�̍쐬
        renderTargetConf.device->CreateRenderTargetView(
            r.Get(), nullptr, rtvHandle
        );
        //���\�[�X���i�[
        resource.push_back(r);

        //�n���h�������炷
        rtvHandle.ptr += this->descriptorHeapSize;
    }

    return resource;
}