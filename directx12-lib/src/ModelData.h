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

    struct Vertices
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT3 normal;
        //UINT color = 0xFFFFFFFF;
        //DirectX::XMFLOAT3 tangent;
    };

    typedef unsigned short USHORT;

public://�擾�n
    const std::vector<Vertices>& getVertices() const { return vertices_; }
    const std::vector<USHORT>& getIndices() const { return indices_; }

public://�ݒ�n
    void setVertices(const std::vector<Vertices>& vertices) { vertices_ = vertices; }
    void setIndices(const std::vector<USHORT>& faces) { indices_ = faces; }

public://�ǉ��n
    void addVertices(const Vertices& vertices) { vertices_.push_back(vertices); }
    void addIndices(const USHORT& faces) { indices_.push_back(faces); }

private:
    std::vector<Vertices> vertices_;
    std::vector<USHORT> indices_;
};
