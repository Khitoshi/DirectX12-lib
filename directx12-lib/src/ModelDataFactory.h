#pragma once
#include "ModelLoader.h"
#include <memory>
#include <string>
class ModelDataFactory
{
public:
    static std::shared_ptr<ModelData> create(ID3D12Device* device, DescriptorHeap* descriptor_heap, const std::string& path)
    {
        return ModelLoader::load(device, descriptor_heap, path.c_str());
    }


};
