#include "IndexBuffer.h"

/// <summary>
/// �C���f�N�X�o�b�t�@�̏�����
/// </summary>
/// <param name="conf">�C���f�b�N�X�o�b�t�@�����p�ݒ�</param>
void IndexBuffer::init(IndexBufferConf conf)
{
    this->sizeInBytes = conf.size;
    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(this->sizeInBytes);

    auto hr = conf.device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &rDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&this->indexBuffer));

    //�C���f�b�N�X�o�b�t�@�̃r���[���쐬�B
    this->indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();

    //�X�g���C�h�͂S�o�C�g�Œ�B
    this->strideInBytes = 4;
    this->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    this->indexBufferView.SizeInBytes = sizeof(uint32_t) * conf.count;;
    this->count = conf.count;
}


/// <summary>
/// ���\�[�X�ɃC���f�b�N�X�f�[�^���R�s�[����B
/// </summary>
/// <param name="srcIndices">�C���f�b�N�X�f�[�^�̔z��</param>
void IndexBuffer::copy(uint16_t* srcIndices)
{
    uint32_t* pData = nullptr;
    if (SUCCEEDED(this->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < count; i++) {
            pData[i] = static_cast<uint32_t>(srcIndices[i]);
        }
    }
    this->indexBuffer->Unmap(0, nullptr);

}

/// <summary>
/// ���\�[�X�ɃC���f�b�N�X�f�[�^���R�s�[����B
/// </summary>
/// <param name="srcIndices">�C���f�b�N�X�f�[�^�̔z��</param>
void IndexBuffer::copy(uint32_t* srcIndices)
{
    uint32_t* pData = nullptr;
    if (SUCCEEDED(this->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < count; i++) {
            pData[i] = srcIndices[i];
        }
    }
    this->indexBuffer->Unmap(0, nullptr);
}
