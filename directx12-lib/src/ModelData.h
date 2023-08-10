#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <Windows.h>
#include <string>

class Texture;

class ModelData
{
public:
    ModelData() :
        vertices_(),
        faces_()
    {};
    ~ModelData() {};

    struct MeshVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT3 normal;
        //UINT color = 0xFFFFFFFF;
        DirectX::XMFLOAT3 tangent;
    };

    struct MeshFace
    {
        UINT index[3];
    };

    struct Material
    {
        DirectX::XMFLOAT3   diffuse;        //ŠgU”½Ë¬•ª
        DirectX::XMFLOAT3   specular;       //‹¾–Ê”½Ë¬•ª
        float               alpha;          //“§‰ß¬•ª
        float               shininess;      //‹¾–Ê”½Ë‹­“x

        std::shared_ptr<Texture> diffuse_texture;
        std::shared_ptr<Texture> specular_texture;
        std::shared_ptr<Texture> shininess_texture;
        std::shared_ptr<Texture> normal_texture;
    };

public://İ’èŒn
    void setVertices(const std::vector<MeshVertex>& vertices)
    {
        this->vertices_ = vertices;
    }

    void setFaces(const std::vector<MeshFace>& faces)
    {
        this->faces_ = faces;
    }

    void setMaterials(const std::vector<Material>& materials)
    {
        this->materials_ = materials;
    }
public://’Ç‰ÁŒn
    void addVertex(const MeshVertex& vertex)
    {
        this->vertices_.push_back(vertex);
    }

    void addFace(const MeshFace& face)
    {
        this->faces_.push_back(face);
    }

    void addMaterial(const Material& material)
    {
        this->materials_.push_back(material);
    }

public://æ“¾Œn
    const std::vector<MeshVertex> getVertices() const { return this->vertices_; }
    const std::vector<MeshFace> getFaces() const { return this->faces_; }
    const std::vector<Material> getMaterials() const { return this->materials_; }
private:
    std::vector<MeshVertex> vertices_;
    std::vector<MeshFace> faces_;
    std::vector<Material> materials_;
};
