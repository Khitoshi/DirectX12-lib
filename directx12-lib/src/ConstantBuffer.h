#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <winerror.h>
#include "Descriptor.h"
using namespace Microsoft::WRL;

//class Descriptor;
class DescriptorHeap;

/// <summary>
/// �萔�o�b�t�@
/// </summary>
class ConstantBuffer :public Descriptor
{
	friend class ConstantBufferFactory;
public:
	struct ConstantBufferConf
	{
		UINT size;                          //�萔�o�b�t�@�̃T�C�Y
		DescriptorHeap* descriptor_heap;    //�f�B�X�N���v�^�q�[�v
		UINT slot;                          //�X���b�g�ԍ�
	};

private:
	ConstantBuffer(const ConstantBufferConf& c) :
		conf_(c)
	{}
public:
	~ConstantBuffer() {}

	ConstantBufferConf getConf()const { return conf_; }
private:
	void init(ID3D12Device* device);
	void createResource(ID3D12Device* device);
	void createView(ID3D12Device* device);

private:
	ConstantBufferConf conf_;
};
