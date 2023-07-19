#pragma once
#include "d3dx12.h"
#include <unordered_map>

class DescriptorHeapCache
{
public:
    DescriptorHeapCache() {}
    ~DescriptorHeapCache() {}

    D3D12_CPU_DESCRIPTOR_HANDLE getOrCreate(ID3D12Device* device, ID3D12Resource* resource, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {
        auto it = handleMapCache.find(resource);
        if (it != handleMapCache.end()) {
            return it->second;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE handle = create(device, resource, descriptorHeap, desc);
        return handle;
    }

private:

    D3D12_CPU_DESCRIPTOR_HANDLE create(ID3D12Device* device, ID3D12Resource* resource, ID3D12DescriptorHeap* descriptorHeap, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * handleMapCache.size();

        device->CreateShaderResourceView(resource, &desc, handle);
        handleMapCache[resource] = handle;

        return handle;
    }

private:

    std::unordered_map<ID3D12Resource*, D3D12_CPU_DESCRIPTOR_HANDLE> handleMapCache;
};
