#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <vector>
#include <string>

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Tangent;
	DirectX::XMFLOAT4 Color;
};

struct Material
{
	std::string diffuse_map_name = "";
	D3D12_GPU_DESCRIPTOR_HANDLE handle_gpu = {};
	DirectX::XMFLOAT4 diffuse_color = { 1,1,1,1 };
};

struct Mesh
{
	std::vector<Vertex> vertices;

	std::vector<unsigned short> indices;
	Material material;
};