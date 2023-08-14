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

        //キャッシュにあるかどうか
        /*
        auto it = fbx_model_cache_.find(model_file_path);
        if (it != fbx_model_cache_.end())
        {
            it->second->
        }
        return FBXModelDataFactory::create(device, model_file_path);

        */
        return nullptr;
    }

private:
    std::unordered_map<const std::string, std::shared_ptr<FBXModelData>> fbx_model_cache_;  //キャッシュ

};
