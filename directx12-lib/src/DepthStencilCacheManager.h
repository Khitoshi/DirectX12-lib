#pragma once

#include "DepthStencilFactory.h"
#include "HashCombine.h"
#include <unordered_map>

/// <summary>
/// �[�x�X�e���V���̃L���b�V�����Ǘ�����N���X
/// </summary>
class DepthStencilCacheManager
{
public:
    /// <summary>
    /// �n�b�V���֐��p�̍\����
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
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns>
    /// �V���O���g���ȃC���X�^���X
    /// </returns>
    static DepthStencilCacheManager& getInstance()
    {
        static DepthStencilCacheManager instance;
        return instance;
    }

    /// <summary>
    /// �ݒ�ɍ��v����DepthStencil���擾����
    /// </summary>
    /// <param name="conf">�����Ɋւ���ݒ�</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&���������ꂽDepthStencil
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
    std::unordered_map<DepthStencil::DepthStencilConf, std::shared_ptr<DepthStencil>, DepthStencilCacheKeyHasher, std::equal_to<DepthStencil::DepthStencilConf>> depth_stencil_cache_;  //�L���b�V��
};
