#include "RenderTarget.h"
#include <stdexcept>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="renderTargetConf"></param>
void RenderTarget::init(RenderTargetConf renderTargetConf)
{
    this->descriptorHeap = createDescriptorHeap(renderTargetConf);
    this->descriptorHeapSize = createDescriptorHeapSize(renderTargetConf);
    this->resource = createResource(renderTargetConf);
}

/// <summary>
/// ディスクリプタヒープの生成
/// </summary>
/// <param name="renderTargetConf">レンダーターゲット生成の設定</param>
/// <returns>
/// 生成したディスクリプタヒープ
/// </returns>
ComPtr<ID3D12DescriptorHeap> RenderTarget::createDescriptorHeap(const RenderTargetConf renderTargetConf)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    desc.NumDescriptors = renderTargetConf.frameBufferCount;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;

    ComPtr<ID3D12DescriptorHeap> dh = nullptr;

    if (FAILED(renderTargetConf.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dh)))) {
        throw std::runtime_error("failed to create render target descriptor heap");
    }

    return dh;
}

/// <summary>
/// ディスクリプタヒープのサイズを取得
/// </summary>
/// <param name="renderTargetConf">レンダーターゲット生成の設定</param>
/// <returns>
/// 取得したディスクリプタヒープのサイズ
/// </returns>
int RenderTarget::createDescriptorHeapSize(RenderTargetConf renderTargetConf)
{
    return renderTargetConf.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

/// <summary>
/// リソースの作成
/// </summary>
/// <param name="renderTargetConf">レンダーターゲット生成の設定</param>
/// <returns>
/// 生成したリソース
/// </returns>
std::vector<ComPtr<ID3D12Resource>> RenderTarget::createResource(const RenderTargetConf renderTargetConf)
{
    //レンダリングターゲットビューのディスクリプタヒープの先頭のハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE  rtvHandle = this->descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart();

    //レンダリングターゲットビューの作成loop
    std::vector<ComPtr<ID3D12Resource>> resource;
    for (UINT n = 0; n < renderTargetConf.frameBufferCount; n++) {
        ComPtr<ID3D12Resource> r = nullptr;
        //スワップチェインからバックバッファを取得
        if (FAILED(renderTargetConf.swapChain->GetBuffer(n, IID_PPV_ARGS(&r)))) {
            throw std::runtime_error("failed to get swap chain buffer");
        }

        //レンダリングターゲットビューの作成
        renderTargetConf.device->CreateRenderTargetView(
            r.Get(), nullptr, rtvHandle
        );
        //リソースを格納
        resource.push_back(r);

        //ハンドルをずらす
        rtvHandle.ptr += this->descriptorHeapSize;
    }

    return resource;
}