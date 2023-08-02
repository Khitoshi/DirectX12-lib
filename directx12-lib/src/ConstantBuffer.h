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
    friend class ConstantBufferFactory;
public:
    struct ConstantBufferConf
    {
        UINT size;      //定数バッファのサイズ
    };

private:
    ConstantBuffer(const ConstantBufferConf& c) :
        conf_(c),
        descriptor_heap_(),
        resource_()
    {}
public:
    ~ConstantBuffer() {}


    /// <summary>
    /// リソースに行列情報をコピーする
    /// </summary>
    /// <param name="matrix">行列情報</param>
    void copy(void* src_constants);

private:
    //初期化処理
    void init(ID3D12Device* device);
    //リソース生成
    void createResource(ID3D12Device* device);
    //ディスクリプタヒープ生成
    void createDescriptorHeap(ID3D12Device* device);
    //ビュー生成
    void createView(ID3D12Device* deviceconf_);

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
    ConstantBufferConf conf_;
    ComPtr<ID3D12Resource>          resource_;               //リソース
    ComPtr<ID3D12DescriptorHeap>    descriptor_heap_;         //ディスクリプタヒープ
};
