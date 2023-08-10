#include "ModelLoader.h"
#include "ModelData.h"
#include "DescriptorHeap.h"
#include <vector>
#include "TextureCacheManager.h"
#include "StringConvertUtil.h"
#include <iostream>
#include <filesystem>

//TODO: ñ¢çÏê¨
std::shared_ptr<ModelData> ModelLoader::load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
{
    return std::shared_ptr<ModelData>();
}
