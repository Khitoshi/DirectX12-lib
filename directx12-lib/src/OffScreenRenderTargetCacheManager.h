#pragma once
#include "OffScreenRenderTargetHashes.h"
#include "OffScreenRenderTarget.h"
#include <stdexcept>
/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g�̃L���b�V���Ǘ��V�X�e��
/// </summary>
class OffScreenRenderTargetCacheManager
{
public:
    struct OffScreenRenderTargetCacheKeyHasher
    {
        std::size_t operator()(const OffScreenRenderTarget::OffScreenRenderTargetConf& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.resourceDesc.Dimension);
            hash_combine(seed, k.resourceDesc.Alignment);
            hash_combine(seed, k.resourceDesc.Width);
            hash_combine(seed, k.resourceDesc.Height);
            hash_combine(seed, k.resourceDesc.DepthOrArraySize);
            hash_combine(seed, k.resourceDesc.MipLevels);
            hash_combine(seed, k.resourceDesc.Format);
            hash_combine(seed, k.resourceDesc.SampleDesc.Count);
            hash_combine(seed, k.resourceDesc.SampleDesc.Quality);
            hash_combine(seed, k.resourceDesc.Layout);
            hash_combine(seed, k.resourceDesc.Flags);

            return seed;
        }
    };
private:
    OffScreenRenderTargetCacheManager() {};
    ~OffScreenRenderTargetCacheManager() {};
public:
    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns></returns>
    static OffScreenRenderTargetCacheManager& getInstance()
    {
        static OffScreenRenderTargetCacheManager instance;
        return instance;
    }

    /// <summary>
    /// �擾or����
    /// </summary>
    /// <param name="device"></param>
    /// <param name="conf"></param>
    /// <returns></returns>
    std::shared_ptr<OffScreenRenderTarget> getOrCreate(ID3D12Device* device, const OffScreenRenderTarget::OffScreenRenderTargetConf& conf) {
        auto it = this->offScreenRenderTargetMapCache.find(conf);
        //�L���b�V���ɂ���Ȃ炻���Ԃ�
        if (it != this->offScreenRenderTargetMapCache.end()) {
            return it->second;
        }

        //����
        create(device, conf);
        it = this->offScreenRenderTargetMapCache.find(conf);
        if (it == this->offScreenRenderTargetMapCache.end()) {
            throw std::runtime_error("OffScreenRenderTargetCacheManager::getOrCreate() OffScreenRenderTarget not found");
        }

        return it->second;
    }

private:
    //�I�t�X�N���[�������_�[�^�[�Q�b�g����
    void create(ID3D12Device* device, const OffScreenRenderTarget::OffScreenRenderTargetConf& conf)
    {
        //�L���b�V���ɂȂ��Ȃ�쐬���ĕԂ�
        std::shared_ptr<OffScreenRenderTarget> offScreenRenderTarget = std::make_shared<OffScreenRenderTarget>(conf);
        offScreenRenderTarget->init(device);
        this->offScreenRenderTargetMapCache[conf] = offScreenRenderTarget;
    }
private:

    std::unordered_map<OffScreenRenderTarget::OffScreenRenderTargetConf, std::shared_ptr<OffScreenRenderTarget>, OffScreenRenderTargetCacheKeyHasher, std::equal_to<OffScreenRenderTarget::OffScreenRenderTargetConf>> offScreenRenderTargetMapCache;
};
