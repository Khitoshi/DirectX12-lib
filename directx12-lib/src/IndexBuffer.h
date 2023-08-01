#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// �C���f�b�N�X�o�b�t�@��copy����N���X
/// </summary>
class IndexBuffer
{
    friend class IndexBufferFactory;
public:
    /// <summary>
    /// �C���f�b�N�X�o�b�t�@�������Ɏg�p����ݒ�
    /// </summary>
    struct IndexBufferConf {
        int size;
        int stride;
        int count;
    };

private:
    IndexBuffer(IndexBufferConf c) :
        conf_(c),
        index_buffer_(),
        index_buffer_view_(),
        stride_in_bytes_(0),
        count_(0),
        size_in_bytes_(0)
    {}

public:
    ~IndexBuffer() {}

private:
    //����������
    void init(ID3D12Device* device);

public:
    //���\�[�X�ɃR�s�[
    void copy(uint16_t* src_indices);
    void copy(uint32_t* src_indices);

public://�擾�n
    //�C���f�b�N�X�o�b�t�@�r���[�̎擾
    const D3D12_INDEX_BUFFER_VIEW& getIndexBufferView() const { return this->index_buffer_view_; }
    //�C���f�b�N�X�o�b�t�@�̃X�g���C�h�̎擾
    int getStrideInBytes() const { return this->stride_in_bytes_; }
    //�C���f�b�N�X�o�b�t�@�̐��̎擾
    int getCount() const { return this->count_; }
    //�C���f�b�N�X�o�b�t�@�̃T�C�Y�̎擾
    int getSizeInBytes() const { return this->size_in_bytes_; }

private:
    IndexBufferConf conf_;
    ComPtr<ID3D12Resource>  index_buffer_;		//�C���f�b�N�X�o�b�t�@�B
    D3D12_INDEX_BUFFER_VIEW index_buffer_view_;	//�C���f�b�N�X�o�b�t�@�r���[
    int stride_in_bytes_;						//�C���f�b�N�X�o�b�t�@�̃X�g���C�h
    int count_;									//�C���f�b�N�X�o�b�t�@�̐�
    int size_in_bytes_;							//�C���f�b�N�X�o�b�t�@�̃T�C�Y
};
