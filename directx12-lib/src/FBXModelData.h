#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <Windows.h>
#include <string>
#include "Hashes.h"
#include <map>
#include <stdexcept>
class Texture;


//fbxモデルデータ
class FBXModelData
{
public:
    //頂点情報構造体
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT3 normal;



        bool operator==(const Vertex& k) const {
            return
                position.x == k.position.x && position.y == k.position.y && position.z == k.position.z &&
                uv.x == k.uv.x && uv.y == k.uv.y &&
                normal.x == k.normal.x && normal.y == k.normal.y && normal.z == k.normal.z;
        }
    };

    typedef unsigned short USHORT;
    //インデックス情報構造体
    struct Index
    {
        USHORT index[3];
    };

    // シェーダーと共有するマテリアルデータ
    struct ShaderMaterial
    {
        DirectX::XMFLOAT4 diffuse_color = DirectX::XMFLOAT4(0, 0, 0, 1);
        //DirectX::XMFLOAT4 specular_color = DirectX::XMFLOAT4(0, 0, 0, 1);

        UINT diffuse_map_index = 0;

        bool operator==(const ShaderMaterial& k) const {
            return diffuse_color.x == k.diffuse_color.x && diffuse_color.y == k.diffuse_color.y &&
                diffuse_map_index == k.diffuse_map_index;
        }
    };

    // アプリケーション内部で使用するマテリアルデータ
    struct Material
    {
        std::string diffuse_map_name;
        std::shared_ptr<Texture> diffuse_map;

        ShaderMaterial shader_material;
        UINT material_index;
        bool operator==(const Material& k) const {
            return
                diffuse_map_name == k.diffuse_map_name &&
                //specular_map_name == k.specular_map_name &&
                shader_material == k.shader_material;
        }
    };

    struct SubSet
    {
        uint64_t unique_id;
        Material material;
    };

    /// <summary>
    /// ハッシュ関数用の構造体
    /// </summary>
    struct ModelDataKeyHasher
    {
        std::size_t operator()(const Vertex& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.position);
            hash_combine(seed, k.uv);
            hash_combine(seed, k.normal);
            return seed;
        }
    };

public:
    FBXModelData() :
        vertices_(),
        indices_()
        //material_()
    {};
    ~FBXModelData() {};

public://取得系
    std::map<uint64_t, std::vector<Vertex>>& getVertices() { return vertices_; }
    const std::vector<Vertex>& getVertex(const uint64_t& i) const {
        auto it = vertices_.find(i);
        if (it == vertices_.end()) throw std::runtime_error("invalid subset id");
        return it->second;
    }

    std::map<uint64_t, std::vector<USHORT>>& getIndices() { return indices_; }
    const std::vector<USHORT>& getIndex(const uint64_t& i) const {
        auto it = indices_.find(i);
        if (it == indices_.end()) throw std::runtime_error("invalid subset id");
        return it->second;
    }

    const std::map<uint64_t, Material>& getSubsetMaps() const { return subset_map_; }
    const Material& getSubsetMap(const uint64_t& i) const {
        auto it = subset_map_.find(i);
        if (it == subset_map_.end()) throw std::runtime_error("invalid subset id");
        return it->second;
    }

public://追加系
    void addVertices(const uint64_t& u, const Vertex& v) { vertices_[u].push_back(v); }
    void addIndices(const uint64_t& u, const USHORT& i) { indices_[u].push_back(i); }
    //void addMaterial(const Material& m) { material_.push_back(m); }
    //void addMaterial(uint64_t u, const Material& m) { material_.push_back(m); }
    void addMaterial(const SubSet& s) { subset_map_[s.unique_id] = s.material; }

private:
    //std::vector <Vertex> vertices_;
    std::map<uint64_t, std::vector<Vertex>> vertices_;
    //std::vector<USHORT> indices_;

    std::map<uint64_t, std::vector<USHORT>> indices_;
    //std::vector<USHORT> indices_;

    //std::vector<Material> material_;
    std::map<uint64_t, Material> subset_map_;

};
