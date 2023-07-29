#pragma once

#include "DepthStencilFactory.h"
#include "HashCombine.h"
#include <unordered_map>

/// <summary>
/// 深度ステンシルのキャッシュを管理するクラス
/// </summary>
class DepthStencilCacheManager
{
public:
    /// <summary>
    /// ハッシュ関数用の構造体
    /// </summary>
    struct DepthStencilCacheKeyHasher
    {
        std::size_t operator()(const DepthStencil::DepthStencilConf& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.frame_buffer_count);
            hash_combine(seed, k.height);
            hash_combine(seed, k.width);
            return seed;
        }
    };

private:
    DepthStencilCacheManager() {};
    ~DepthStencilCacheManager() {}
public:

    /// <summary>
    /// シングルトンなインスタンスを取得
    /// </summary>
    /// <returns>
    /// シングルトンなインスタンス
    /// </returns>
    static DepthStencilCacheManager& getInstance()
    {
        static DepthStencilCacheManager instance;
        return instance;
    }

    /// <summary>
    /// 設定に合致するDepthStencilを取得する
    /// </summary>
    /// <param name="conf">生成に関する設定</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化されたDepthStencil
    /// </returns>
    std::shared_ptr<DepthStencil> getOrCreate(const DepthStencil::DepthStencilConf& conf, ID3D12Device* device)
    {
        auto itr = depth_stencil_cache_.find(conf);
        if (itr != depth_stencil_cache_.end())
        {
            return itr->second;
        }

        auto depth_stencil = DepthStencilFactory::create(conf, device);
        depth_stencil_cache_[conf] = depth_stencil;
        return depth_stencil;
    }

private:
    std::unordered_map<DepthStencil::DepthStencilConf, std::shared_ptr<DepthStencil>, DepthStencilCacheKeyHasher, std::equal_to<DepthStencil::DepthStencilConf>> depth_stencil_cache_;  //キャッシュ
};
