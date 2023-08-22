#pragma once

#include "Mesh.h"
#include "AssimpLoader.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "HashCombine.h"

class ModelMeshCacheManager
{
private:
    struct MeshConf
    {
        std::string model_file_path = "";
        bool inverse_u = false;
        bool inverse_v = false;

        bool operator==(const MeshConf& rhs) const
        {
            return
                model_file_path == rhs.model_file_path &&
                inverse_u == rhs.inverse_u &&
                inverse_v == rhs.inverse_v;
        }
    };

    struct MeshConfHasher
    {
        std::size_t operator()(const MeshConf& key) const
        {
            std::size_t seed = 0;
            hash_combine(seed, key.model_file_path);
            hash_combine(seed, key.inverse_u);
            hash_combine(seed, key.inverse_v);
            return seed;
        }
    };

public:

    //シングルトンなインスタンスを取得
    static ModelMeshCacheManager& getInstance()
    {
        static ModelMeshCacheManager instance;
        return instance;
    }


    std::vector<Mesh> getMeshes(const std::string model_file_path, bool inverse_u = false, bool inverse_v = false)
    {
        MeshConf conf;
        conf.model_file_path = model_file_path;
        conf.inverse_u = inverse_u;
        conf.inverse_v = inverse_v;

        auto ite = mesh_caches_.find(conf);
        if (ite != mesh_caches_.end()) {
            return ite->second;
        }

        auto meshes = AssimpLoader::Load(model_file_path.c_str(), inverse_u, inverse_v);
        mesh_caches_[conf] = meshes;
        return meshes;

    }

private:
    std::unordered_map <MeshConf, std::vector<Mesh>, MeshConfHasher, std::equal_to<MeshConf>> mesh_caches_;
};