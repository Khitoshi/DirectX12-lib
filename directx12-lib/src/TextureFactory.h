#pragma once

#include "Texture.h"
#include <memory>

/// <summary>
/// テクスチャのファクトリクラス
/// </summary>
class TextureFactory
{
public:
	static std::shared_ptr<Texture> create(ID3D12Device* device, const char* texture_file_path)
	{
		std::shared_ptr<Texture> texture(new Texture());
		texture->Load(device, texture_file_path);
		return texture;
	}

};
