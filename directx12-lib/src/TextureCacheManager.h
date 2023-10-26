#pragma once

#include "TextureFactory.h"
class DescriptorHeap;

struct aiMaterial;

/// <summary>
/// テクスチャのキャッシュを管理するクラス
/// </summary>
class TextureCacheManager
{
private:
	TextureCacheManager() {};
	~TextureCacheManager() {};

public:
	static TextureCacheManager& getInstance()
	{
		static TextureCacheManager instance;
		return instance;
	}

	std::shared_ptr<Texture> getOrCreate(ID3D12Device* device, const char* path)
	{
		auto it = this->texture_caches_.find(path);
		if (it != this->texture_caches_.end()) {
			return it->second;
		}

		auto texture = TextureFactory::create(device, path);
		this->texture_caches_[path] = texture;
		return texture;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Texture>> texture_caches_;
};
