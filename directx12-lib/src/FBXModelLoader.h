#pragma once
#include "d3dx12.h"
#include <memory>

class FBXModelData;
class DescriptorHeap;

//fbxモデルのローダークラス
class FBXModelLoader
{
private:
    FBXModelLoader() {};
    ~FBXModelLoader() {};
public:
    //fbxモデルをロードする
    static std::shared_ptr<FBXModelData> load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path);
};
