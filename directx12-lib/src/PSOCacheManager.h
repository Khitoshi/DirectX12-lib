#pragma once

#include "PipelineStateObjectFactory.h"
#include "Hashes.h"
#include <stdexcept>
#include <map>
#include <unordered_map>

class PSOCacheManager
{
public:

    struct PSOParametorHasher
    {
        std::size_t operator()(const PipelineStateObject::PipelineStateObjectConf& c) const
        {
            std::size_t seed = 0;
            hash_combine(seed, c.desc.pRootSignature);
            hash_combine(seed, c.desc.VS);
            hash_combine(seed, c.desc.PS);
            hash_combine(seed, c.desc.HS);
            hash_combine(seed, c.desc.GS);
            hash_combine(seed, c.desc.DS);
            hash_combine(seed, c.desc.BlendState);
            hash_combine(seed, c.desc.RasterizerState);
            hash_combine(seed, c.desc.DepthStencilState);
            hash_combine(seed, c.desc.PrimitiveTopologyType);
            hash_combine(seed, c.desc.NumRenderTargets);
            hash_combine(seed, c.desc.RTVFormats[0]);
            hash_combine(seed, c.desc.RTVFormats[1]);
            hash_combine(seed, c.desc.RTVFormats[2]);
            hash_combine(seed, c.desc.RTVFormats[3]);
            hash_combine(seed, c.desc.RTVFormats[4]);
            hash_combine(seed, c.desc.RTVFormats[5]);
            hash_combine(seed, c.desc.RTVFormats[6]);
            hash_combine(seed, c.desc.RTVFormats[7]);
            hash_combine(seed, c.desc.DSVFormat);
            hash_combine(seed, c.desc.SampleDesc);
            return seed;
        }
    };

private:
    PSOCacheManager() :
        pso_cache_()
    {};
    ~PSOCacheManager() {};
public:

    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns></returns>
    static PSOCacheManager& getInstance()
    {
        static PSOCacheManager instance;
        return instance;
    }

    std::shared_ptr<PipelineStateObject> getOrCreate(ID3D12Device* device, const PipelineStateObject::PipelineStateObjectConf conf) {
        auto it = this->pso_cache_.find(conf);
        // �L���b�V���ɂ��邩�m�F
        if (it != this->pso_cache_.end()) {
            // �L���b�V���ɂ���ꍇ�͂����Ԃ�
            return it->second;
        }

        // �L���b�V���ɂȂ��ꍇ�͐V�K�쐬
        std::shared_ptr<PipelineStateObject> pso = PipelineStateObjectFactory::create(conf, device);
        this->pso_cache_[conf] = pso;

        return pso;
    }

private:
    std::unordered_map<PipelineStateObject::PipelineStateObjectConf, std::shared_ptr<PipelineStateObject>, PSOParametorHasher, std::equal_to<PipelineStateObject::PipelineStateObjectConf>> pso_cache_;
};
