#pragma once
#include "FBXModelLoader.h"
#include <memory>
#include <string>
class FBXModelDataFactory
{
public:
    static std::shared_ptr<FBXModelData> create(ID3D12Device* device, DescriptorHeap* descriptor_heap, const std::string& path)
    {
        return FBXModelLoader::load(device, descriptor_heap, path.c_str());
    }
};
