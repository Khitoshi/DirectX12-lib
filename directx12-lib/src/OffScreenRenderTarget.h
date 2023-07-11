#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

class OffScreenRenderTarget
{
public:
    struct OffScreenRenderTargetConf {
        //UINT width;
		//UINT height;
		//DXGI_FORMAT format;
		//D3D12_RESOURCE_FLAGS flags;
        D3D12_RESOURCE_DESC backBufferDesc;
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	};
    
public:
    OffScreenRenderTarget(OffScreenRenderTargetConf c) :conf(c), resource(){};
    ~OffScreenRenderTarget() {};

    void init(ID3D12Device* device);
private:
    //���\�[�X���쐬
    void createResource(ID3D12Device* device);
    //�f�B�X�N���v�^�q�[�v���쐬
    void createDescriptorHeap(ID3D12Device* device);
    //�����_�[�^�[�Q�b�g�r���[���쐬
    void createView(ID3D12Device* device);
    //�f�B�X�N���v�^�q�[�v�̃T�C�Y���쐬
    void createDescriptorHeapSize(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[���쐬
    void createShaderResourceView(ID3D12Device* device);
public:
    ID3D12Resource* getResource() const { return resource.Get(); }  				    //���\�[�X�̎擾
    ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptorHeap.Get(); }    //�f�B�X�N���v�^�q�[�v�̎擾
    int getDescriptorHeapSize() const { return descriptorHeapSize; }                    //�f�B�X�N���v�^�q�[�v�̃T�C�Y�̎擾
private:
    OffScreenRenderTargetConf conf;				    //�ݒ�
    ComPtr<ID3D12Resource> resource;				//���\�[�X
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //�f�B�X�N���v�^�q�[�v
    int descriptorHeapSize;                         //�f�B�X�N���v�^�q�[�v�̃T�C�Y
};
