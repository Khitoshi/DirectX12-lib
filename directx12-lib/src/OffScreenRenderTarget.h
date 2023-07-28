#pragma once
#include "d3dx12.h"
#include "RenderContext.h"
#include "DepthStencil.h"

using namespace Microsoft::WRL;

/// <summary>
/// �}���`�p�X�����_�����O�p�̃I�t�X�N���[�������_�[�^�[�Q�b�g
/// </summary>
class OffScreenRenderTarget
{
public:
    struct OffScreenRenderTargetConf {
        D3D12_RESOURCE_DESC resourceDesc; 		        //�o�b�N�o�b�t�@�̐ݒ�
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;  //�o�b�N�o�b�t�@�Ŏg�p���Ă���f�B�X�N���v�^�q�[�v�̐ݒ�

        bool operator==(const OffScreenRenderTargetConf& conf) const {
            return
                resourceDesc.Width == conf.resourceDesc.Width &&
                resourceDesc.Height == conf.resourceDesc.Height &&
                resourceDesc.Format == conf.resourceDesc.Format &&
                resourceDesc.DepthOrArraySize == conf.resourceDesc.DepthOrArraySize &&
                resourceDesc.MipLevels == conf.resourceDesc.MipLevels &&
                resourceDesc.SampleDesc.Count == conf.resourceDesc.SampleDesc.Count &&
                resourceDesc.SampleDesc.Quality == conf.resourceDesc.SampleDesc.Quality &&
                resourceDesc.Dimension == conf.resourceDesc.Dimension &&
                resourceDesc.Layout == conf.resourceDesc.Layout &&
                resourceDesc.Flags == conf.resourceDesc.Flags &&

                descriptorHeapDesc.NumDescriptors == conf.descriptorHeapDesc.NumDescriptors &&
                descriptorHeapDesc.Type == conf.descriptorHeapDesc.Type;
        }
    };

public:
    OffScreenRenderTarget(OffScreenRenderTargetConf c) :
        conf(c),
        resource(),
        SRVHeap(),
        RTVHeap()
    {};
    ~OffScreenRenderTarget() {};

    //������
    void init(ID3D12Device* device);
    //�����_�����O�J�n����
    void beginRender(RenderContext* rc);
    //�����_�����O�I������
    void endRender(RenderContext* rc);

private:
    //���\�[�X���쐬
    void createResource(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[�̃f�B�X�N���v�^�q�[�v���쐬
    void createSRVHeap(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[���쐬
    void createShaderResourceView(ID3D12Device* device);
    //�����_�[�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�q�[�v���쐬
    void createRTVHeap(ID3D12Device* device);
    //�����_�[�^�[�Q�b�g�r���[���쐬
    void createRenderTargetView(ID3D12Device* device);
public://�ݒ�n
    void setDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE cdh) { this->depthStencilViewHandle = cdh; } //�[�x�X�e���V���̐ݒ�
    void setViewport(D3D12_VIEWPORT vp) { viewport = vp; }  //�r���[�|�[�g�̐ݒ�
public://�擾�n
    ID3D12Resource* getResource() const { return resource.Get(); }      //���\�[�X�̎擾
    ID3D12DescriptorHeap* getSRVHeap() const { return SRVHeap.Get(); }  //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* getRTVHeap() const { return RTVHeap.Get(); }  //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v�̎擾
private:
    OffScreenRenderTargetConf conf;			//�ݒ�
    ComPtr<ID3D12Resource> resource;		//���\�[�X
    ComPtr<ID3D12DescriptorHeap> SRVHeap;   //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> RTVHeap;   //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v

    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle; //�[�x�X�e���V���r���[�n���h��
    D3D12_VIEWPORT viewport;                            //�r���[�|�[�g

};
