#pragma once
#include "Shader.h"

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
};

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

