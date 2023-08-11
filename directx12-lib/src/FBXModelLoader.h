#pragma once
#include "d3dx12.h"
#include <memory>

class FBXModelData;
class DescriptorHeap;

//fbx���f���̃��[�_�[�N���X
class FBXModelLoader
{
private:
    FBXModelLoader() {};
    ~FBXModelLoader() {};
public:
    //fbx���f�������[�h����
    static std::shared_ptr<FBXModelData> load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path);
};
