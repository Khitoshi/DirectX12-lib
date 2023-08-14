#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <winerror.h>

using namespace Microsoft::WRL;

class DescriptorHeap;

/// <summary>
/// �萔�o�b�t�@
/// </summary>
class ConstantBuffer
{
    friend class ConstantBufferFactory;
public:
    struct ConstantBufferConf
    {
        UINT size;      //�萔�o�b�t�@�̃T�C�Y
        DescriptorHeap* descriptor_heap; //�f�B�X�N���v�^�q�[�v
        UINT slot; //�X���b�g�ԍ�
    };

private:
    ConstantBuffer(const ConstantBufferConf& c) :
        conf_(c),
        resource_()
    {}
public:
    ~ConstantBuffer() {}


    /// <summary>
    /// ���\�[�X�ɍs������R�s�[����
    /// </summary>
    /// <param name="matrix">�s����</param>
    void copy(void* src_constants);

private:
    //����������
    void init(ID3D12Device* device);
    //���\�[�X����
    void createResource(ID3D12Device* device);
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
public:
    ConstantBufferConf conf_;
    ComPtr<ID3D12Resource> resource_; //���\�[�X
};
