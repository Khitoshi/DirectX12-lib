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
        indices_()
    {};
    ~ModelData() {};

    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT3 normal;
        //UINT color = 0xFFFFFFFF;
        //DirectX::XMFLOAT3 tangent;
        bool operator==(const Vertex& k) const {
            return
                position.x == k.position.x && position.y == k.position.y && position.z == k.position.z &&
                uv.x == k.uv.x && uv.y == k.uv.y &&
                normal.x == k.normal.x && normal.y == k.normal.y && normal.z == k.normal.z;
        }
    };

    typedef unsigned short USHORT;
    struct Index
    {
        USHORT index[3];
    };


public://Žæ“¾Œn
    const std::vector<Vertex>& getVertices() const { return vertices_; }
    const std::vector<USHORT>& getIndices() const { return indices_; }

public://’Ç‰ÁŒn
    void addVertices(const Vertex& vertices) { vertices_.push_back(vertices); }
    void addIndices(const USHORT& faces) { indices_.push_back(faces); }

private:
    std::vector <Vertex> vertices_;
    std::vector<USHORT> indices_;
};
