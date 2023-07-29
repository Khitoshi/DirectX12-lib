#pragma once
#include "d3dx12.h"
#include <map>

class DescriptorHeapCache
{
public:
    /// <summary>
    /// キャッシュ生成時の設定
    /// </summary>
    struct DescriptorHeapCacheConf
    {
        ID3D12Resource* resource;   //リソース
        UINT slot;                  //スロット
    };

public:
    DescriptorHeapCache() {}
    ~DescriptorHeapCache() {}

    /// <summary>
    /// 取得or作成
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="conf">生成時の設定</param>
    /// <param name="descriptorHeap">ディスクリプタヒープ</param>
    /// <param name="desc">SRVの設定</param>
    void getOrCreate(ID3D12Device* device, DescriptorHeapCacheConf conf, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {
        //すでに作成済みの場合は何もしない
        auto it = this->handleMapCache_.find(conf.slot);
        if (it != this->handleMapCache_.end() && it->second == conf.resource) {
            return;
        }

        //作成
        create(device, conf, descriptorHeap, desc);
    }

private:

    /// <summary>
    /// リソース作成
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="conf">生成時の設定</param>
    /// <param name="descriptorHeap">ディスクリプタヒープ</param>
    /// <param name="desc">SRVの設定</param>
    void create(ID3D12Device* device, DescriptorHeapCacheConf conf, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += static_cast<unsigned long long>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * conf.slot;

        device->CreateShaderResourceView(conf.resource, &desc, handle);
        handleMapCache_[conf.slot] = conf.resource;
    }

private:
    std::map<UINT, ID3D12Resource*> handleMapCache_;    //slotをキーにしたリソースのマップ
};