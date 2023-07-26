#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <winerror.h>
using namespace Microsoft::WRL;

/// <summary>
/// 定数バッファ
/// </summary>
class ConstantBuffer
{
public:
    ConstantBuffer() : resource_() {}
    ~ConstantBuffer() {}

    //初期化処理
    void init(ID3D12Device* device);

    /// <summary>
    /// リソースに行列情報をコピーする
    /// </summary>
    /// <param name="matrix">行列情報</param>
    void copy(DirectX::XMMATRIX* matrix)
    {
        DirectX::XMMATRIX* mat = nullptr;
        if (FAILED(this->resource_->Map(0, nullptr, (void**)&mat))) {
            throw std::exception("ConstantBuffer::copy() : resource->Map() Failed.");
        }
        memcpy(mat, matrix, sizeof(DirectX::XMMATRIX));
        this->resource_->Unmap(0, nullptr);
    }

private:
    //リソース生成
    void createResource(ID3D12Device* device);
    //ディスクリプタヒープ生成
    void createDescriptorHeap(ID3D12Device* device);
    //ビュー生成
    void createView(ID3D12Device* device);

public://取得系
    /// <summary>
    /// 定数バッファのGPU仮想アドレスを取得する
    /// </summary>
    /// <returns>定数バッファのGPU仮想アドレス</returns>
    D3D12_GPU_VIRTUAL_ADDRESS getConstantBufferViewGPUVirtualAddress()
    {
        return this->resource_->GetGPUVirtualAddress();
    }

    /// <summary>
    /// ディスクリプタヒープの開始アドレスを取得する
    /// </summary>
    /// <returns>ディスクリプタヒープの開始アドレス</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE getGPUDescriptorHandleForHeapStart()
    {
        return this->descriptor_heap_->GetGPUDescriptorHandleForHeapStart();
    }

    /// <summary>
    /// ディスクリプタヒープ取得
    /// </summary>
    /// <returns>ディスクリプタヒープ</returns>
    ID3D12DescriptorHeap* getDescriptorHeap()
    {
        return this->descriptor_heap_.Get();
    }

public:
    ComPtr<ID3D12Resource>          resource_;               //リソース
    ComPtr<ID3D12DescriptorHeap>    descriptor_heap_;         //ディスクリプタヒープ

};
