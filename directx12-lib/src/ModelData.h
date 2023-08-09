#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <Windows.h>
#include <string>

class ModelData
{
public:
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


};
