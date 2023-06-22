#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

struct IndexBufferConf {
	ID3D12Device* device;
	int size;
	int stride;
};

class IndexBuffer
{
public:
	IndexBuffer():
		indexBuffer(),
		indexBufferView(),
		strideInBytes(0),
		count(0),
		sizeInBytes(0)
	
	{}
	~IndexBuffer() {}

	void init(IndexBufferConf conf);
	void copy(uint16_t* srcIndices);
	void copy(uint32_t* srcIndices);

public:
	//�C���f�b�N�X�o�b�t�@�r���[�̎擾
	D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const { return indexBufferView; }

	//�C���f�b�N�X�o�b�t�@�̃X�g���C�h�̎擾
	int getStrideInBytes() const { return strideInBytes; }

	//�C���f�b�N�X�o�b�t�@�̐��̎擾
	int getCount() const { return count; }

	//�C���f�b�N�X�o�b�t�@�̃T�C�Y�̎擾
	int getSizeInBytes() const { return sizeInBytes; }
private:
	ID3D12Resource* indexBuffer = nullptr;		//�C���f�b�N�X�o�b�t�@�B
	D3D12_INDEX_BUFFER_VIEW indexBufferView;	//�C���f�b�N�X�o�b�t�@�r���[�B
	int strideInBytes;							//�C���f�b�N�X�o�b�t�@�̃X�g���C�h�B
	int count;									//�C���f�b�N�X�o�b�t�@�̐��B
	int sizeInBytes;							//�C���f�b�N�X�o�b�t�@�̃T�C�Y�B
};
