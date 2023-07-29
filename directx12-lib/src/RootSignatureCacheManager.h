#pragma once
#include "RootSignatureFactory.h"
#include "Shader.h"
#include "HashCombine.h"
#include <unordered_map>

/// <summary>
/// ���[�g�V�O�l�`���̃L���b�V�����Ǘ�����N���X
/// </summary>
class RootSignatureCacheManager
{
public:
    /// <summary>
    /// �n�b�V���֐��p�̍\����
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
    /// �V���O���g���ȃC���X�^���X���擾����
    /// </summary>
    /// <returns>�V���O���g���ȃC���X�^���X</returns>
    static RootSignatureCacheManager& getInstance()
    {
        static RootSignatureCacheManager instance;
        return instance;
    }

    /// <summary>
    /// �擾���������[�g�V�O�l�`���̐ݒ���w�肵��,���[�g�V�O�l�`�����擾����
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <param name="conf">�ݒ�</param>
    /// <returns></returns>
    std::shared_ptr<RootSignature> getOrCreate(ID3D12Device* device, const RootSignature::RootSignatureConf& conf) {
        auto it = this->root_signature_map_cache_.find(conf);

        //�L���b�V���ɂ���Ȃ炻���Ԃ�
        if (it != this->root_signature_map_cache_.end()) {
            return it->second;
        }

        //���݂��Ȃ��ꍇ,�쐬���ĕԂ�
        std::shared_ptr<RootSignature> rootSignature = RootSignatureFactory::create(device, conf);
        this->root_signature_map_cache_[conf] = rootSignature;
        return rootSignature;
    }

private:
    //���[�g�V�O�l�`���̃L���b�V��
    std::unordered_map<RootSignature::RootSignatureConf, std::shared_ptr<RootSignature>, RootSignatureCacheKeyHasher, std::equal_to<RootSignature::RootSignatureConf>> root_signature_map_cache_;
};
