#pragma once

#include "TextureFactory.h"
class DescriptorHeap;

/// <summary>
/// テクスチャのキャッシュを管理するクラス
/// </summary>
class TextureCacheManager
{
private:
    TextureCacheManager() {};
    ~TextureCacheManager() {};

public:

    /// <summary>
    /// シングルトンなインスタンスを取得
    /// </summary>
    /// <returns>シングルトンなインスタンス</returns>
    static TextureCacheManager& getInstance()
    {
        static TextureCacheManager instance;
        return instance;
    }

    /// <summary>
    /// キャッシュを確認して、なければ作って返す
    /// </summary>
    /// <param name="device"></param>
    /// <param name="path"></param>
    /// <returns></returns>
    std::shared_ptr<Texture> getOrCreate(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* path)
    {
        // すでにあるならそれを返す
        auto it = this->texture_caches_.find(path);
        if (it != this->texture_caches_.end()) {
            return it->second;
        }

        // ないなら作って返す
        auto texture = TextureFactory::create(device, descriptor_heap, path);
        this->texture_caches_[path] = texture;
        return texture;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> texture_caches_;  // テクスチャのキャッシュ
};
