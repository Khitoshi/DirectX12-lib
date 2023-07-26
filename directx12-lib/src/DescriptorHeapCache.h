#pragma once
#include "d3dx12.h"
#include <map>

class DescriptorHeapCache
{
public:
    struct DescriptorHeapCacheConf
    {
        ID3D12Resource* resource;
        UINT slot;
    };


public:
    DescriptorHeapCache() {}
    ~DescriptorHeapCache() {}

    void getOrCreate(ID3D12Device* device, DescriptorHeapCacheConf conf, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {

        auto it = handleMapCache.find(conf.slot);
        if (it != handleMapCache.end() && it->second == conf.resource) {
            return;
        }

        create(device, conf, descriptorHeap, desc);
    }

private:

    void create(ID3D12Device* device, DescriptorHeapCacheConf conf, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * conf.slot;

        device->CreateShaderResourceView(conf.resource, &desc, handle);
        handleMapCache[conf.slot] = conf.resource;
    }

private:

    std::map<UINT, ID3D12Resource*> handleMapCache;

};