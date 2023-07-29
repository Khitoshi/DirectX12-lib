#include <stdexcept>
#include "OffScreenRenderTarget.h"
#include "RenderContext.h"
#include "DepthStencil.h"
#include "CommonGraphicsConfig.h"

/// <summary>
/// 初期化
/// </summary>
/// <param name="device">初期化&生成済みのGPUデバイス</param>
void OffScreenRenderTarget::init(ID3D12Device* device)
{
    createResource(device);
    createSRVHeap(device);
    createShaderResourceView(device);
    createRTVHeap(device);
    createRenderTargetView(device);
}

/// <summary>
/// offscreenに描画をする為の初期処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
void OffScreenRenderTarget::beginRender(RenderContext* rc)
{
    rc->transitionOffScreenRenderTargetBegin(this->resource_.Get());
    rc->simpleStart(this->rtv_heap_->GetCPUDescriptorHandleForHeapStart(), this->depth_stencil_view_handle_);
}

/// <summary>
/// offscreenに描画をする為の終了処理
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
void OffScreenRenderTarget::endRender(RenderContext* rc)
{
    //レンダーターゲットのRESOURCE_BARRIER設定
    rc->transitionOffScreenRenderTargetEnd(this->resource_.Get());
}

/// <summary>
/// リソースを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createResource(ID3D12Device* device)
{
    D3D12_RESOURCE_DESC desc = {};
    desc = this->conf_.resource_desc;
    D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_CLEAR_VALUE clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, backGroundColor);

    if (FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clear_value,
        IID_PPV_ARGS(this->resource_.ReleaseAndGetAddressOf())
    ))) {
        throw std::runtime_error("failed to create offscreen render target");
    }
}

/// <summary>
/// シェーダーリソースビュー用のディスクリプタヒープを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createSRVHeap(ID3D12Device* device)
{
    //SRVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC dh_desc = {};
    dh_desc = this->conf_.descriptor_heap_desc;
    dh_desc.NumDescriptors = 2;
    dh_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    dh_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&dh_desc, IID_PPV_ARGS(this->srv_heap_.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// シェーダーリソースビューを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createShaderResourceView(ID3D12Device* device)
{
    //シェーダーリソースビューを作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    device->CreateShaderResourceView(this->resource_.Get(), &srv_desc, this->srv_heap_->GetCPUDescriptorHandleForHeapStart());
}

/// <summary>
/// レンダーターゲットビュー用のディスクリプタヒープを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createRTVHeap(ID3D12Device* device)
{
    //RTVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC dh_desc = {};
    dh_desc = this->conf_.descriptor_heap_desc;
    dh_desc.NumDescriptors = 1;
    if (FAILED(device->CreateDescriptorHeap(&dh_desc, IID_PPV_ARGS(this->rtv_heap_.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// レンダーターゲットビューを作成する
/// </summary>
/// <param name="device">GPUデバイス</param>
void OffScreenRenderTarget::createRenderTargetView(ID3D12Device* device)
{
    //レンダーターゲットビューを作成
    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
    rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateRenderTargetView(this->resource_.Get(), &rtv_desc, this->rtv_heap_->GetCPUDescriptorHandleForHeapStart());
}