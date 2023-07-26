#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <winerror.h>
using namespace Microsoft::WRL;

/// <summary>
/// �萔�o�b�t�@
/// </summary>
class ConstantBuffer
{
public:
    ConstantBuffer() : resource_() {}
    ~ConstantBuffer() {}

    //����������
    void init(ID3D12Device* device);

    /// <summary>
    /// ���\�[�X�ɍs������R�s�[����
    /// </summary>
    /// <param name="matrix">�s����</param>
    void copy(DirectX::XMMATRIX* matrix)
    {
        DirectX::XMMATRIX* mat = nullptr;
        if (FAILED(this->resource_->Map(0, nullptr, (void**)&mat))) {
            throw std::exception("ConstantBuffer::copy() : resource->Map() Failed.");
        }
        memcpy(mat, matrix, sizeof(DirectX::XMMATRIX));
        this->resource_->Unmap(0, nullptr);
    }

private:
    //���\�[�X����
    void createResource(ID3D12Device* device);
    //�f�B�X�N���v�^�q�[�v����
    void createDescriptorHeap(ID3D12Device* device);
    //�r���[����
    void createView(ID3D12Device* device);

public://�擾�n
    /// <summary>
    /// �萔�o�b�t�@��GPU���z�A�h���X���擾����
    /// </summary>
    /// <returns>�萔�o�b�t�@��GPU���z�A�h���X</returns>
    D3D12_GPU_VIRTUAL_ADDRESS getConstantBufferViewGPUVirtualAddress()
    {
        return this->resource_->GetGPUVirtualAddress();
    }

    /// <summary>
    /// �f�B�X�N���v�^�q�[�v�̊J�n�A�h���X���擾����
    /// </summary>
    /// <returns>�f�B�X�N���v�^�q�[�v�̊J�n�A�h���X</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE getGPUDescriptorHandleForHeapStart()
    {
        return this->descriptor_heap_->GetGPUDescriptorHandleForHeapStart();
    }

    /// <summary>
    /// �f�B�X�N���v�^�q�[�v�擾
    /// </summary>
    /// <returns>�f�B�X�N���v�^�q�[�v</returns>
    ID3D12DescriptorHeap* getDescriptorHeap()
    {
        return this->descriptor_heap_.Get();
    }

public:
    ComPtr<ID3D12Resource>          resource_;               //���\�[�X
    ComPtr<ID3D12DescriptorHeap>    descriptor_heap_;         //�f�B�X�N���v�^�q�[�v

};
