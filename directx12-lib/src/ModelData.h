#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <Windows.h>
#include <string>

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
        DirectX::XMFLOAT3   diffuse;        //拡散反射成分
        DirectX::XMFLOAT3   specular;       //鏡面反射成分
        float               alpha;          //透過成分
        float               shininess;      //鏡面反射強度
        std::wstring        diffuse_map;     //ディフューズマップファイルパス
        std::wstring        specular_map;    //スペキュラーマップファイルパス
        std::wstring        shininess_map;   //シャイネスマップファイルパス
        std::wstring        normal_map;      //法線マップファイルパス
    };

public://設定系
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
public://追加系
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

public://取得系
    const std::vector<MeshVertex> getVertices() const { return this->vertices_; }
    const std::vector<MeshFace> getFaces() const { return this->faces_; }
    const std::vector<Material> getMaterials() const { return this->materials_; }
private:
    std::vector<MeshVertex> vertices_;
    std::vector<MeshFace> faces_;
    std::vector<Material> materials_;
};
