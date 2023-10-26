#pragma once
#include "ShaderFactory.h"
#include <map>
#include <unordered_map>

/// <summary>
/// シェーダーのキャッシュを管理するクラス
/// </summary>
class ShaderCacheManager
{
private:
	struct ShaderConfHash
	{
		size_t operator()(const Shader::ShaderConf& shaderConf) const
		{
			std::hash<std::string> stringHasher;
			size_t filePathHash = stringHasher(shaderConf.file_path);
			size_t entryFuncNameHash = stringHasher(shaderConf.entry_func_name);
			size_t shaderModelTypeHash = std::hash<int>()(static_cast<int>(shaderConf.current_shader_model_type));

			return filePathHash ^ (entryFuncNameHash << 1) ^ (shaderModelTypeHash << 2);
		}
	};

private:
	ShaderCacheManager() {};
	~ShaderCacheManager() {};

public:
	static ShaderCacheManager& getInstance()
	{
		static ShaderCacheManager instance;
		return instance;
	}

	std::shared_ptr<Shader> getOrCreate(const Shader::ShaderConf& conf);

private:
	std::unordered_map<Shader::ShaderConf, std::shared_ptr<Shader>, ShaderConfHash> shader_cache_;
};
