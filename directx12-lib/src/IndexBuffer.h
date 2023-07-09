#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// �C���f�b�N�X�o�b�t�@�������Ɏg�p����ݒ�
/// </summary>
struct IndexBufferConf {
    ID3D12Device* device;
    int size;
    int stride;
    int count;
};

/// <summary>
/// �C���f�b�N�X�o�b�t�@�����p&copy����N���X
/// </summary>
class IndexBuffer
{
public:
    IndexBuffer() :
        indexBuffer(),
        indexBufferView(),
        strideInBytes(0),
        count(0),
        sizeInBytes(0)

    {}
    ~IndexBuffer() {}

    //����������
    void init(IndexBufferConf conf);

    //���\�[�X�ɃR�s�[
    void copy(uint16_t* srcIndices);
    void copy(uint32_t* srcIndices);

public:
    //�C���f�b�N�X�o�b�t�@�r���[�̎擾
    const D3D12_INDEX_BUFFER_VIEW& getIndexBufferView() const { return indexBufferView; }

    //�C���f�b�N�X�o�b�t�@�̃X�g���C�h�̎擾
    int getStrideInBytes() const { return strideInBytes; }

    //�C���f�b�N�X�o�b�t�@�̐��̎擾
    int getCount() const { return count; }

    //�C���f�b�N�X�o�b�t�@�̃T�C�Y�̎擾
    int getSizeInBytes() const { return sizeInBytes; }
private:
    ComPtr<ID3D12Resource>  indexBuffer;		//�C���f�b�N�X�o�b�t�@�B
    D3D12_INDEX_BUFFER_VIEW indexBufferView;	//�C���f�b�N�X�o�b�t�@�r���[�B
    int strideInBytes;							//�C���f�b�N�X�o�b�t�@�̃X�g���C�h�B
    int count;									//�C���f�b�N�X�o�b�t�@�̐��B
    int sizeInBytes;							//�C���f�b�N�X�o�b�t�@�̃T�C�Y�B
};
