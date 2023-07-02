#pragma once
#include "Shader.h"
#include <map>
#include <unordered_map>

class ShaderCacheManager
{
private:
    ShaderCacheManager() {};
    ~ShaderCacheManager() {};
public:

    static ShaderCacheManager& getInstance()
    {
        static ShaderCacheManager instance;
        return instance;
    }

    std::shared_ptr<Shader> getOrCreate(ShaderConf conf)
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

private:
    void create(ShaderConf conf)
    {
        std::shared_ptr<Shader> shader = std::make_shared<Shader>();
        shader->load(conf);
        shaderCache[conf] = shader;
    }

private:
    //std::map<std::string, std::shared_ptr<Shader>> shaderCache;
    std::unordered_map<ShaderConf, std::shared_ptr<Shader>, ShaderConfHash> shaderCache;

};
