#pragma once
#include "RootSignature.h"
#include "Shader.h"
#include "HashCombine.h"
#include <unordered_map>



class RootSignatureCacheManager
{
public:
    struct RootSignatureCacheKeyHasher
    {
        std::size_t operator()(const RootSignatureConf& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.samplerFilter);
            hash_combine(seed, k.textureAddressModeU);
            hash_combine(seed, k.textureAddressModeV);
            hash_combine(seed, k.textureAddressModeW);
            hash_combine(seed, k.numSampler);
            hash_combine(seed, k.maxCbvDescriptor);
            hash_combine(seed, k.maxSrvDescriptor);
            hash_combine(seed, k.maxUavDescriptor);
            hash_combine(seed, k.offsetInDescriptorsFromTableStartCB);
            hash_combine(seed, k.offsetInDescriptorsFromTableStartSRV);
            hash_combine(seed, k.offsetInDescriptorsFromTableStartUAV);
            hash_combine(seed, k.rootSignatureFlags);
            return seed;
        }
    };

private:
    RootSignatureCacheManager() {};
    ~RootSignatureCacheManager() {};

public:
    static RootSignatureCacheManager& getInstance()
    {
        static RootSignatureCacheManager instance;
        return instance;
    }

    std::shared_ptr<RootSignature> getOrCreate(ID3D12Device* device, const RootSignatureConf& conf) {

        auto it = this->rootSignatureMapCache.find(conf);

        //キャッシュにあるならそれを返す
        if (it != this->rootSignatureMapCache.end()) {
            return it->second;
        }

        std::shared_ptr<RootSignature> rootSignature = this->createRootSignature(device, conf);
        this->rootSignatureMapCache[conf] = rootSignature;
        return rootSignature;
    }

private:
    //ルートシグネチャ生成
    std::shared_ptr<RootSignature> createRootSignature(ID3D12Device* device, const RootSignatureConf& conf)
    {
        std::shared_ptr<RootSignature> rootSignature = std::make_shared<RootSignature>();
        rootSignature->init(device, conf);
        return rootSignature;
    }

private:

    std::unordered_map<RootSignatureConf, std::shared_ptr<RootSignature>, RootSignatureCacheKeyHasher, std::equal_to<RootSignatureConf>> rootSignatureMapCache;
};
