#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <winerror.h>

using namespace Microsoft::WRL;

class DescriptorHeap;

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
        DescriptorHeap* descriptor_heap; //ディスクリプタヒープ
        UINT slot; //スロット番号
    };

private:
    ConstantBuffer(const ConstantBufferConf& c) :
        conf_(c),
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
public:
    ConstantBufferConf conf_;
    ComPtr<ID3D12Resource> resource_; //リソース
};
