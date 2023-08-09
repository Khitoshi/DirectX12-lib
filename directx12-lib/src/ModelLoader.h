#pragma once
#include "d3dx12.h"
#include <memory>

class ModelData;
class DescriptorHeap;

class ModelLoader
{
private:
    ModelLoader() {};
    ~ModelLoader() {};
public:
    static std::shared_ptr<ModelData> load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path);
};