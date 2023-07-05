#include "ShaderCacheManager.h"

namespace std {
    
    template <>
    struct hash<ShaderConf> {
        size_t operator()(const ShaderConf& shaderConf) const
        {
            std::hash<std::string> stringHasher;
            size_t filePathHash = stringHasher(shaderConf.filePath);
            size_t entryFuncNameHash = stringHasher(shaderConf.entryFuncName);
            size_t shaderModelTypeHash = std::hash<int>()(static_cast<int>(shaderConf.currentShaderModelType));

            // Combined hash of all values
            return filePathHash ^ (entryFuncNameHash << 1) ^ (shaderModelTypeHash << 2);
        }
    };
}

std::shared_ptr<Shader> ShaderCacheManager::getOrCreate(ShaderConf conf)
{
    auto it = shaderCache.find(conf);
    //シェーダーがキャッシュにある場合
    if (it != shaderCache.end())
    {
        return it->second;
    }

    //生成
    create(conf);
    return shaderCache[conf];
}

void ShaderCacheManager::create(ShaderConf conf)
{
    std::shared_ptr<Shader> shader = std::make_shared<Shader>();
    shader->load(conf);
    shaderCache[conf] = shader;
}