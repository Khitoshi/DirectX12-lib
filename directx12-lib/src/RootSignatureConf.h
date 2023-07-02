#pragma once
#include "d3dx12.h"
#include <vector>
#include <d3d12.h>


struct MyStruct
{
    ID3D12Device* device;
    std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplerDescArray;
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags;

    size_t hash() const {

        size_t seed = 0;

        for (auto& param : rootParameters) {
            hash_combine(seed, std::hash<CD3DX12_ROOT_PARAMETER1>()(param));
        }

        return 0;
    }
};

