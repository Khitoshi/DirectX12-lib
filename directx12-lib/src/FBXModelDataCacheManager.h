#pragma once

#include "FBXModelData.h"
#include "FBXModelDataFactory.h"

class FBXModelDataCacheManager
{
private:
    FBXModelDataCacheManager() {};
    ~FBXModelDataCacheManager() {};
public:
    /// <summary>
    /// シングルトンなインスタンスを取得
    /// </summary>
    /// <returns>
    /// シングルトンなインスタンス
    /// </returns>
    static FBXModelDataCacheManager& getInstance()
    {
        static FBXModelDataCacheManager instance;
        return instance;
    }

    std::shared_ptr<FBXModelData> getOrCreate(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
    {
        //TODO:現在テクスチャではディスクリプタヒープにすぐに入れているのをリソースの作成だけにして
        //手動でディスクリプタヒープに入れるようにする


        return FBXModelDataFactory::create(device, descriptor_heap, model_file_path);
    }


private:

};
