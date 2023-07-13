#include <stdexcept>
#include "OffScreenRenderTarget.h"

/// <summary>
/// ������
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void OffScreenRenderTarget::init(ID3D12Device* device)
{
    createResource(device);
    createSRVHeap(device);
    createShaderResourceView(device);
    createRTVHeap(device);
    createRenderTargetView(device);
}

/// <summary>
/// offscreen�ɕ`�������ׂ̏�������
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
void OffScreenRenderTarget::beginRender(RenderContext* rc)
{
    //�r���[�|�[�g�ƃV�U�����O��`�̐ݒ�
    rc->setViewport(this->viewport);
    rc->setScissorRect(this->viewport);

    //�����_�[�^�[�Q�b�g��RESOURCE_BARRIER�ݒ�
    rc->TransitionTemporaryRenderTargetBegin(this->resource.Get());

    //�����_�[�^�[�Q�b�g��ݒ�
    rc->setRenderTarget(this->RTVHeap->GetCPUDescriptorHandleForHeapStart(), this->depthStencilViewHandle);
    //�����_�[�^�[�Q�b�g�̃N���A
    rc->clearRenderTarget(this->RTVHeap->GetCPUDescriptorHandleForHeapStart(), conf.clearColor);
    //�[�x�X�e���V���̃N���A
    rc->clearDepthStencil(depthStencilViewHandle, 1.0f);


}

/// <summary>
/// offscreen�ɕ`�������ׂ̏I������
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
void OffScreenRenderTarget::endRender(RenderContext* rc)
{
    //�����_�[�^�[�Q�b�g��RESOURCE_BARRIER�ݒ�
    //this->renderContext->TransitionTemporaryRenderTargetAwait(this->offScreenRenderTarget->getResource());
    //this->offScreenRenderTarget->endRender(this->renderContext.get());
    rc->TransitionTemporaryRenderTargetAwait(this->resource.Get());
}

/// <summary>
/// ���\�[�X���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void OffScreenRenderTarget::createResource(ID3D12Device* device)
{
    D3D12_RESOURCE_DESC desc = {};
    desc = this->conf.resourceDesc;
    D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, conf.clearColor);

    if (FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(this->resource.ReleaseAndGetAddressOf())
    ))) {
        throw std::runtime_error("failed to create offscreen render target");
    }
}

/// <summary>
/// �V�F�[�_�[���\�[�X�r���[�p�̃f�B�X�N���v�^�q�[�v���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void OffScreenRenderTarget::createSRVHeap(ID3D12Device* device)
{
    //SRV�f�B�X�N���v�^�q�[�v�쐬
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf.descriptorHeapDesc;
    desc.NumDescriptors = 2;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->SRVHeap.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// �V�F�[�_�[���\�[�X�r���[���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void OffScreenRenderTarget::createShaderResourceView(ID3D12Device* device)
{
    //�V�F�[�_�[���\�[�X�r���[���쐬
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    device->CreateShaderResourceView(this->resource.Get(), &srvDesc, this->SRVHeap->GetCPUDescriptorHandleForHeapStart());
}

/// <summary>
/// �����_�[�^�[�Q�b�g�r���[�p�̃f�B�X�N���v�^�q�[�v���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void OffScreenRenderTarget::createRTVHeap(ID3D12Device* device)
{
    //RTV�f�B�X�N���v�^�q�[�v�쐬
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf.descriptorHeapDesc;
    desc.NumDescriptors = 1;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->RTVHeap.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// �����_�[�^�[�Q�b�g�r���[���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void OffScreenRenderTarget::createRenderTargetView(ID3D12Device* device)
{
    //�����_�[�^�[�Q�b�g�r���[���쐬
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateRenderTargetView(this->resource.Get(), &rtvDesc, this->RTVHeap->GetCPUDescriptorHandleForHeapStart());
}
