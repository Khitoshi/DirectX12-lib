#include "ShaderCacheManager.h"

namespace std {
    template <>
    struct hash<Shader::ShaderConf> {
        size_t operator()(const Shader::ShaderConf& shaderConf) const
        {
            std::hash<std::string> stringHasher;
            size_t filePathHash = stringHasher(shaderConf.file_path);
            size_t entryFuncNameHash = stringHasher(shaderConf.entry_func_name);
            size_t shaderModelTypeHash = std::hash<int>()(static_cast<int>(shaderConf.current_shader_model_type));

            // Combined hash of all values
            return filePathHash ^ (entryFuncNameHash << 1) ^ (shaderModelTypeHash << 2);
        }
    };
}

/// <summary>
/// シェーダーを取得する
/// </summary>
/// <param name="conf"></param>
/// <returns></returns>
std::shared_ptr<Shader> ShaderCacheManager::getOrCreate(const Shader::ShaderConf& conf)
{
    auto it = this->shader_cache_.find(conf);
    //シェーダーがキャッシュにある場合
    if (it != this->shader_cache_.end())
    {
        return it->second;
    }

    //生成
    auto shader = ShaderFactory::create(conf);
    shader_cache_[conf] = shader;
    return shader;
}