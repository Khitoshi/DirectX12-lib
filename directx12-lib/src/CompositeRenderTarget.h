#pragma once
#include "d3dx12.h"
#include "PipelineStateObject.h"
#include "VertexBuffer.h"
#include "RenderContext.h"
#include "RootSignature.h"
#include "Shader.h"
#include "DescriptorHeapCache.h"

class CompositeRenderTarget
{
public:
    struct CompositeRenderTargetConf {
        D3D12_RESOURCE_DESC resourceDesc; 		    //�o�b�N�o�b�t�@�̐ݒ�
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;  //�o�b�N�o�b�t�@�Ŏg�p���Ă���f�B�X�N���v�^�q�[�v�̐ݒ�
        float clearColor[4];                            //�������̃N���A�J���[
    };

public:
    CompositeRenderTarget(CompositeRenderTargetConf c) :
        conf(c),
        resource(),
        SRVHeap(),
        RTVHeap(),
        srvDesc(),
        descriptorHeapCache()
    {};
    ~CompositeRenderTarget() {};

    //������
    void init(ID3D12Device* device);
    void beginRender(RenderContext* rc, D3D12_VIEWPORT viewport, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle);
    void render(RenderContext* rc, ID3D12Device* device);
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

    //���[�g�V�O�l�`���̍쐬
    void initRootSignature(ID3D12Device* device);
    //�V�F�[�_�[�̃��[�h
    void initShader(ID3D12Device* device);
    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
    void initPipelineStateObject(ID3D12Device* device);

    //���_�o�b�t�@�̍쐬
    void initVertexBuffer(ID3D12Device* device);

public://�ݒ�n
    //void setDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE cdh) { this->depthStencilViewHandle = cdh; }       //�[�x�X�e���V���̐ݒ�
    //void setViewport(D3D12_VIEWPORT vp) { viewport = vp; }             //�r���[�|�[�g�̐ݒ�
public://�擾�n
    ID3D12Resource* getResource() const { return resource.Get(); }      //���\�[�X�̎擾
    ID3D12DescriptorHeap* getSRVHeap() const { return SRVHeap.Get(); }  //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v�̎擾
    ID3D12DescriptorHeap* getRTVHeap() const { return RTVHeap.Get(); }  //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v�̎擾
private:
    //���_�f�[�^
    struct Vertex
    {
        DirectX::XMFLOAT3 position;	//���W
        DirectX::XMFLOAT2 uv;		//�e�N�X�`�����W
    };

    CompositeRenderTargetConf conf;			            //�ݒ�
    ComPtr<ID3D12Resource> resource;		            //���\�[�X
    ComPtr<ID3D12DescriptorHeap> SRVHeap;               //�V�F�[�_�[���\�[�X�r���[�f�B�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> RTVHeap;               //�����_�[�^�[�Q�b�g�r���[�f�B�X�N���v�^�q�[�v

    std::shared_ptr<PipelineStateObject> pso;           //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    std::shared_ptr<VertexBuffer> vb;                   //���_�o�b�t�@
    std::shared_ptr<RootSignature> rootSignature;       //���[�g�V�O�j�`��
    BasicShaderPair shaderPair;                         //�V�F�[�_�[�y�A
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    std::shared_ptr<DescriptorHeapCache> descriptorHeapCache;       //�f�B�X�N���v�^�q�[�v�L���b�V���}�l�[�W���[
};
