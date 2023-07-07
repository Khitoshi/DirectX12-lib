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
    ConstantBuffer(): resource(){}
    ~ConstantBuffer() {}

    void init(ID3D12Device* device);

    /// <summary>
    /// リソースに行列情報をコピーする
    /// </summary>
    /// <param name="matrix">行列情報</param>
    void copy(DirectX::XMMATRIX* matrix)
    {
        DirectX::XMMATRIX* mat = nullptr;
        if (FAILED(this->resource->Map(0, nullptr, (void**)&mat))){
            throw std::exception("ConstantBuffer::copy() : resource->Map() Failed.");
        }
        memcpy(mat, matrix, sizeof(DirectX::XMMATRIX));
        this->resource->Unmap(0, nullptr);
    }

private:
    void createResource(ID3D12Device* device);
    void createDescriptorHeap(ID3D12Device* device);
    void createView(ID3D12Device* device);
public:
    D3D12_GPU_VIRTUAL_ADDRESS getConstantBufferViewGPUVirtualAddress()
    {
        return this->resource->GetGPUVirtualAddress();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE getGPUDescriptorHandleForHeapStart()
    {
		return this->descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}

    ID3D12DescriptorHeap* getDescriptorHeap()
    {
        return this->descriptorHeap.Get();
    }

public:
    ComPtr<ID3D12Resource>          resource;               //リソース  
    ComPtr<ID3D12DescriptorHeap>    descriptorHeap;         //ディスクリプタヒープ
};
