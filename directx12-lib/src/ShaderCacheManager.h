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

    std::shared_ptr<Shader> getOrCreate(ShaderConf conf);

private:
    void create(ShaderConf conf);

private:
    //std::map<std::string, std::shared_ptr<Shader>> shaderCache;
    std::unordered_map<ShaderConf, std::shared_ptr<Shader>, ShaderConfHash> shaderCache;

};
