#pragma once
#include "d3dx12.h"

class RenderContext;
class DepthStencil;

using namespace Microsoft::WRL;

/// <summary>
/// �}���`�p�X�����_�����O�p�̃I�t�X�N���[�������_�[�^�[�Q�b�g
/// </summary>
class OffScreenRenderTarget
{
    friend class OffScreenRenderTargetFactory;
public:
    /// <summary>
    /// �����Ɏg�p����ݒ�
    /// </summary>
    struct OffScreenRenderTargetConf {
        D3D12_RESOURCE_DESC resource_desc; 		            //�o�b�N�o�b�t�@�̐ݒ�
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;    //�o�b�N�o�b�t�@�Ŏg�p���Ă���f�B�X�N���v�^�q�[�v�̐ݒ�

        bool operator==(const OffScreenRenderTargetConf& conf) const {
            return
                resource_desc.Width == conf.resource_desc.Width &&
                resource_desc.Height == conf.resource_desc.Height &&
                resource_desc.Format == conf.resource_desc.Format &&
                resource_desc.DepthOrArraySize == conf.resource_desc.DepthOrArraySize &&
                resource_desc.MipLevels == conf.resource_desc.MipLevels &&
                resource_desc.SampleDesc.Count == conf.resource_desc.SampleDesc.Count &&
                resource_desc.SampleDesc.Quality == conf.resource_desc.SampleDesc.Quality &&
                resource_desc.Dimension == conf.resource_desc.Dimension &&
                resource_desc.Layout == conf.resource_desc.Layout &&
                resource_desc.Flags == conf.resource_desc.Flags &&

                descriptor_heap_desc.NumDescriptors == conf.descriptor_heap_desc.NumDescriptors &&
                descriptor_heap_desc.Type == conf.descriptor_heap_desc.Type;
        }
    };

private:
    OffScreenRenderTarget(OffScreenRenderTargetConf c) :
        conf_(c),
        resource_(),
        srv_heap_(),
        rtv_heap_(),
        depth_stencil_view_handle_(),
        viewport_()
    {};

public:
    ~OffScreenRenderTarget() {};

    //�����_�����O�J�n����
    void beginRender(RenderContext* rc);
    //�����_�����O�I������
    void endRender(RenderContext* rc);

private:
    //������
    void init(ID3D12Device* device);
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
    //�[�x�X�e���V���̐ݒ�
    void setDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE cdh) { this->depth_stencil_view_handle_ = cdh; }
    //�r���[�|�[�g�̐ݒ�
    void setViewport(const D3D12_VIEWPORT vp) { this->viewport_ = vp; }

public://�擾�n
    //���\�[�X�̎擾
    ID3D12Resource* getResource()       const { return this->resource_.Get(); }
    //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* getSRVHeap()  const { return this->srv_heap_.Get(); }
    //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* getRTVHeap()  const { return this->rtv_heap_.Get(); }

private:
    OffScreenRenderTargetConf conf_;			            //�ݒ�
    ComPtr<ID3D12Resource> resource_;		                //���\�[�X
    ComPtr<ID3D12DescriptorHeap> srv_heap_;                 //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> rtv_heap_;                 //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v

    D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_view_handle_; //�[�x�X�e���V���r���[�n���h��
    D3D12_VIEWPORT viewport_;                               //�r���[�|�[�g
};
