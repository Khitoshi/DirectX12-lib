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
        UINT color = 0xFFFFFFFF;
        DirectX::XMFLOAT3 tangent;
    };

public:

    struct Node
    {
        std::shared_ptr<Mesh>	spMesh;	// ���b�V��
    };

    /// <summary>
    /// ���f���̃��[�h
    /// </summary>
    /// <param name="filepath">�t�@�C���p�X</param>
    /// <returns>����������true</returns>
    bool Load(const std::string& filepath);

    /// <summary>
    /// �m�[�h�̎擾
    /// </summary>
    /// <returns>�m�[�h���</returns>
    const std::vector<Node>& GetNodes()const { return m_nodes; }

private:

    std::vector<Node>		m_nodes;
};