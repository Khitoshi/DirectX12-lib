#include "RenderTarget.h"
#include <stdexcept>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="renderTargetConf">RTV生成用設定</param>
void RenderTarget::init(ID3D12Device* device)
{
    createDescriptorHeap(device);
    createDescriptorHeapSize(device);
    createResource(device);
}

/// <summary>
/// ディスクリプタヒープの生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void RenderTarget::createDescriptorHeap(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = this->conf_.frame_buffer_count;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;

    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->descriptor_heap_)))) {
        throw std::runtime_error("failed to create render target descriptor heap");
    }
}

/// <summary>
/// ディスクリプタヒープのサイズを設定
/// </summary>
/// <param name="device">GPUデバイス</param>
void RenderTarget::createDescriptorHeapSize(ID3D12Device* device)
{
    this->descriptor_heap_size_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

/// <summary>
/// リソースの生成
/// </summary>
/// <param name="device">GPUデバイス</param>
void RenderTarget::createResource(ID3D12Device* device)
{
    //レンダリングターゲットビューのディスクリプタヒープの先頭のハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE  rtvHandle = this->descriptor_heap_.Get()->GetCPUDescriptorHandleForHeapStart();

    //レンダリングターゲットビューの作成loop
    for (UINT n = 0; n < this->conf_.frame_buffer_count; n++) {
        ComPtr<ID3D12Resource> r = {};
        //スワップチェインからバックバッファを取得
        if (FAILED(this->conf_.swap_chain->GetBuffer(n, IID_PPV_ARGS(&r)))) {
            throw std::runtime_error("failed to get swap chain buffer");
        }

        //レンダリングターゲットビューの作成
        device->CreateRenderTargetView(
            r.Get(), nullptr, rtvHandle
        );

        //リソースを格納
        this->resource_.push_back(r);

        //ハンドルをずらす
        rtvHandle.ptr += this->descriptor_heap_size_;
    }
}