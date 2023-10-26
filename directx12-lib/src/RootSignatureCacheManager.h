#pragma once
#include "RootSignatureFactory.h"
#include "Shader.h"
#include "HashCombine.h"
#include <unordered_map>

class RootSignatureCacheManager
{
public:
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
			hash_combine(seed, k.num_cbv);
			hash_combine(seed, k.num_srv);
			hash_combine(seed, k.num_uav);
			hash_combine(seed, k.root_signature_flags);
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

	std::shared_ptr<RootSignature> getOrCreate(ID3D12Device* device, const RootSignature::RootSignatureConf& conf) {
		auto it = this->root_signature_map_cache_.find(conf);
		if (it != this->root_signature_map_cache_.end()) {
			return it->second;
		}

		std::shared_ptr<RootSignature> rootSignature = RootSignatureFactory::create(device, conf);
		this->root_signature_map_cache_[conf] = rootSignature;
		return rootSignature;
	}

private:
	std::unordered_map<RootSignature::RootSignatureConf, std::shared_ptr<RootSignature>, RootSignatureCacheKeyHasher, std::equal_to<RootSignature::RootSignatureConf>> root_signature_map_cache_;
};
