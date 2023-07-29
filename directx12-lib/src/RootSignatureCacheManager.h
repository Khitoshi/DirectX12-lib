#pragma once
#include "RootSignatureFactory.h"
#include "Shader.h"
#include "HashCombine.h"
#include <unordered_map>

/// <summary>
/// ルートシグネチャのキャッシュを管理するクラス
/// </summary>
class RootSignatureCacheManager
{
public:
    /// <summary>
    /// ハッシュ関数用の構造体
    /// </summary>
    struct RootSignatureCacheKeyHasher
    {
        std::size_t operator()(const RootSignature::RootSignatureConf& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.sampler_filter);
            hash_combine(seed, k.texture_address_modeU);
            hash_combine(seed, k.texture_address_modeV);
            hash_combine(seed, k.texture_address_modeW);
            hash_combine(seed, k.num_sampler);
            hash_combine(seed, k.max_cbv_descriptor);
            hash_combine(seed, k.max_srv_descriptor);
            hash_combine(seed, k.max_uav_descriptor);
            hash_combine(seed, k.offset_in_descriptors_from_table_start_cb);
            hash_combine(seed, k.offset_in_descriptors_from_table_start_srv);
            hash_combine(seed, k.offset_in_descriptors_from_table_start_uav);
            hash_combine(seed, k.root_signature_flags);
            return seed;
        }
    };

private:
    RootSignatureCacheManager() {};
    ~RootSignatureCacheManager() {};

public:
    /// <summary>
    /// シングルトンなインスタンスを取得する
    /// </summary>
    /// <returns>シングルトンなインスタンス</returns>
    static RootSignatureCacheManager& getInstance()
    {
        static RootSignatureCacheManager instance;
        return instance;
    }

    /// <summary>
    /// 取得したいルートシグネチャの設定を指定して,ルートシグネチャを取得する
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="conf">設定</param>
    /// <returns></returns>
    std::shared_ptr<RootSignature> getOrCreate(ID3D12Device* device, const RootSignature::RootSignatureConf& conf) {
        auto it = this->root_signature_map_cache_.find(conf);

        //キャッシュにあるならそれを返す
        if (it != this->root_signature_map_cache_.end()) {
            return it->second;
        }

        //存在しない場合,作成して返す
        std::shared_ptr<RootSignature> rootSignature = RootSignatureFactory::create(device, conf);
        this->root_signature_map_cache_[conf] = rootSignature;
        return rootSignature;
    }

private:
    //ルートシグネチャのキャッシュ
    std::unordered_map<RootSignature::RootSignatureConf, std::shared_ptr<RootSignature>, RootSignatureCacheKeyHasher, std::equal_to<RootSignature::RootSignatureConf>> root_signature_map_cache_;
};
