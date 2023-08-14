#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <Windows.h>
#include <string>
#include "Hashes.h"
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
        DirectX::XMFLOAT4 diffuse_color;

        bool operator==(const ShaderMaterial& k) const {
            return diffuse_color.x == k.diffuse_color.x && diffuse_color.y == k.diffuse_color.y && 
                diffuse_color.z == k.diffuse_color.z && diffuse_color.w == k.diffuse_color.w;
        }
    };

    // アプリケーション内部で使用するマテリアルデータ
    struct Material
    {
        std::string diffuse_texture_name;
        std::shared_ptr<Texture> texture;
        ShaderMaterial shader_material;

        bool operator==(const Material& k) const {
            return diffuse_texture_name == k.diffuse_texture_name &&
                shader_material == k.shader_material;
        }
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
        indices_(),
        material_()
    {};
    ~FBXModelData() {};

public://取得系
    const std::vector<Vertex>& getVertices() const { return vertices_; }
    const std::vector<USHORT>& getIndices() const { return indices_; }
    std::vector<Material> getMaterial() const { return material_; }
public://設定系
    void setVertices(const std::vector<Vertex>& v) { vertices_ = v; }
    void setIndices(const std::vector<USHORT>& i) { indices_ = i; }
    //void setDiffuseTexture(const std::shared_ptr<Texture>& d) { diffuse_texture_ = d; }

    std::vector<ShaderMaterial> getShaderMaterials() const {
        std::vector<ShaderMaterial> shaderMaterials;
        for (const auto& mat : material_) {
            shaderMaterials.push_back(mat.shader_material);
        }
        return shaderMaterials;
    }


public://追加系
    void addVertices(const Vertex& v) { vertices_.push_back(v); }
    void addIndices(const USHORT& i) { indices_.push_back(i); }
    void addMaterial(const Material& m) { material_.push_back(m); }
private:
    std::vector <Vertex> vertices_;
    std::vector<USHORT> indices_;
    std::vector<Material> material_;

    //std::shared_ptr<Texture> diffuse_texture_;

};
