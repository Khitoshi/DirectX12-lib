#include "IndexBuffer.h"

/// <summary>
/// �C���f�N�X�o�b�t�@�̏�����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void IndexBuffer::init(ID3D12Device* device)
{
    this->size_in_bytes_ = this->conf_.size;
    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(this->size_in_bytes_);

    auto hr = device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &rDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&this->index_buffer_));

    //�C���f�b�N�X�o�b�t�@�̃r���[���쐬�B
    this->index_buffer_view_.BufferLocation = index_buffer_->GetGPUVirtualAddress();

    //�X�g���C�h�͂S�o�C�g�Œ�B
    this->stride_in_bytes_ = 4;
    this->index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
    this->index_buffer_view_.SizeInBytes = sizeof(uint32_t) * this->conf_.count;;
    this->count_ = this->conf_.count;
}

/// <summary>
/// ���\�[�X�ɃC���f�b�N�X�f�[�^���R�s�[����B
/// </summary>
/// <param name="srcIndices">�C���f�b�N�X�f�[�^�̔z��</param>
void IndexBuffer::copy(uint16_t* src_indices)
{
    uint32_t* pData = nullptr;
    if (SUCCEEDED(this->index_buffer_->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < count_; i++) {
            pData[i] = static_cast<uint32_t>(src_indices[i]);
        }
    }
    this->index_buffer_->Unmap(0, nullptr);
}

/// <summary>
/// ���\�[�X�ɃC���f�b�N�X�f�[�^���R�s�[����B
/// </summary>
/// <param name="srcIndices">�C���f�b�N�X�f�[�^�̔z��</param>
void IndexBuffer::copy(uint32_t* src_indices)
{
    uint32_t* pData = nullptr;
    if (SUCCEEDED(this->index_buffer_->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        for (int i = 0; i < this->count_; i++) {
            pData[i] = src_indices[i];
        }
    }
    this->index_buffer_->Unmap(0, nullptr);
}

void IndexBuffer::copy(void* src_indices)
{
    uint8_t* pData = nullptr;
    if (SUCCEEDED(this->index_buffer_->Map(0, nullptr, reinterpret_cast<void**>(&pData)))) {
        /*
        for (int i = 0; i < this->count_; i++) {
            pData[i] = src_indices[i];
            //���_�f�[�^���}�b�s���O��ɓo�^
        }
        */
        memcpy(pData, src_indices, this->index_buffer_view_.SizeInBytes);
    }
    this->index_buffer_->Unmap(0, nullptr);
}
