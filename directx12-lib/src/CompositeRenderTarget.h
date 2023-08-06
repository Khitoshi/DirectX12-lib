#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "d3dx12.h"

class   RootSignature;
class   DescriptorHeap;
class   RenderContext;
class   VertexBuffer;
class   PipelineStateObject;
class   DescriptorCache;
class   Shader;

using Microsoft::WRL::ComPtr;

/// <summary>
/// �I�t�X�N���[����1���̃e�N�X�`���ɍ�������N���X
/// </summary>
class CompositeRenderTarget
{
    friend class CompositeRenderTargetFactory;
public:
    //���������ɕK�v�ȏ��
    struct CompositeRenderTargetConf {
        D3D12_RESOURCE_DESC resource_desc; 		            //�o�b�N�o�b�t�@�̐ݒ�
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;    //�o�b�N�o�b�t�@�Ŏg�p���Ă���f�B�X�N���v�^�q�[�v�̐ݒ�
    };

private:
    CompositeRenderTarget(CompositeRenderTargetConf c) :
        conf_(c),
        resource_(),
        cbv_srv_uav_descriptor_heap_(),
        rtv_descriptor_heap_(),
        pso_(),
        vertex_buffer_(),
        root_signature_(),
        pixel_shader_(),
        vertex_shader_(),
        srv_desc_(),
        descriptor_cache_()
    {};
public:
    ~CompositeRenderTarget() {};

    //�`�揉������
    void beginRender(RenderContext* rc, D3D12_CPU_DESCRIPTOR_HANDLE depthStencil_view_handle);
    //�`�揈��
    void render(RenderContext* rc, ID3D12Device* device);
    //�`��I������
    void endRender(RenderContext* rc);

private:
    //������
    void init(ID3D12Device* device);
    //���\�[�X���쐬
    void createResource(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[�̃f�B�X�N���v�^�q�[�v���쐬
    void createSRVHeap(ID3D12Device* device);
    //�V�F�[�_�[���\�[�X�r���[���쐬
    void createSRVDesc(ID3D12Device* device);
    //�����_�[�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�q�[�v���쐬
    void createRTVHeap(ID3D12Device* device);
    //�����_�[�^�[�Q�b�g�r���[���쐬
    void createRTV(ID3D12Device* device);

    //���[�g�V�O�l�`���̍쐬
    void initRootSignature(ID3D12Device* device);
    //�V�F�[�_�[�̃��[�h
    void initShader(ID3D12Device* device);
    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
    void initPipelineStateObject(ID3D12Device* device);
    //���_�o�b�t�@�̍쐬
    void initVertexBuffer(ID3D12Device* device);
    //�f�B�X�N���v�^�q�[�v�L���b�V���}�l�[�W���[�̍쐬
    void initDescriptorHeapCache();

public://�擾�n
    //���\�[�X�̎擾
    ID3D12Resource* getResource() const { return resource_.Get(); }

    //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v�̎擾
    //ID3D12DescriptorHeap* getSRVHeap() const { return cbv_srv_uav_descriptor_heap_->getDescriptorHeap(); }

    //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* getRTVHeap() const;
private:
    //���_�f�[�^
    struct Vertex
    {
        DirectX::XMFLOAT3 position;	//���W
        DirectX::XMFLOAT2 uv;		//�e�N�X�`�����W
    };

    CompositeRenderTargetConf conf_;			                    //�N���X�̐ݒ�

    ComPtr<ID3D12Resource> resource_;		                        //���\�[�X
    std::shared_ptr<DescriptorHeap> cbv_srv_uav_descriptor_heap_;   //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v
    std::shared_ptr<DescriptorHeap> rtv_descriptor_heap_;           //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v

    std::shared_ptr<PipelineStateObject> pso_;                      //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vertex_buffer_;                   //���_�o�b�t�@
    std::shared_ptr<RootSignature> root_signature_;                 //���[�g�V�O�j�`��
    std::shared_ptr<Shader> pixel_shader_;                          //�s�N�Z���V�F�[�_�[
    std::shared_ptr<Shader> vertex_shader_;                         //���_�V�F�[�_�[
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_;                      //�V�F�[�_�[���\�[�X�r���[�̐ݒ�

    std::shared_ptr<DescriptorCache> descriptor_cache_;             //�f�B�X�N���v�^�L���b�V��
};
