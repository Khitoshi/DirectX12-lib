#pragma once
#include "HashCombine.h"
#include "d3dx12.h"
#include <DirectXMath.h>

/// <summary>
/// 値計算用のハッシュ関数
/// </summary>
namespace std {
	template <>
	struct hash<D3D12_BLEND_DESC> {
		std::size_t operator()(const D3D12_BLEND_DESC& blendDesc) const {
			std::size_t seed = 0;
			hash_combine(seed, blendDesc.AlphaToCoverageEnable);
			hash_combine(seed, blendDesc.IndependentBlendEnable);
			for (const auto& rtBlendDesc : blendDesc.RenderTarget) {
				hash_combine(seed, rtBlendDesc);
			}
			return seed;
		}
	};

	template<>
	struct hash<D3D12_RASTERIZER_DESC>
	{
		std::size_t operator()(const D3D12_RASTERIZER_DESC& rDesc)const {
			std::size_t seed = 0;
			hash_combine(seed, rDesc.AntialiasedLineEnable);
			hash_combine(seed, rDesc.ConservativeRaster);
			hash_combine(seed, rDesc.CullMode);
			hash_combine(seed, rDesc.DepthBias);
			hash_combine(seed, rDesc.DepthBiasClamp);
			hash_combine(seed, rDesc.DepthClipEnable);
			hash_combine(seed, rDesc.FillMode);
			hash_combine(seed, rDesc.ForcedSampleCount);
			hash_combine(seed, rDesc.FrontCounterClockwise);
			hash_combine(seed, rDesc.MultisampleEnable);
			hash_combine(seed, rDesc.SlopeScaledDepthBias);
			return seed;
		}
	};

	template<>
	struct hash<D3D12_RENDER_TARGET_BLEND_DESC>
	{
		std::size_t operator()(const D3D12_RENDER_TARGET_BLEND_DESC& rtDesc)const {
			std::size_t seed = 0;
			hash_combine(seed, rtDesc.BlendEnable);
			hash_combine(seed, rtDesc.BlendOp);
			hash_combine(seed, rtDesc.BlendOpAlpha);
			hash_combine(seed, rtDesc.DestBlend);
			hash_combine(seed, rtDesc.DestBlendAlpha);
			hash_combine(seed, rtDesc.LogicOp);
			hash_combine(seed, rtDesc.LogicOpEnable);
			hash_combine(seed, rtDesc.RenderTargetWriteMask);
			hash_combine(seed, rtDesc.SrcBlend);
			hash_combine(seed, rtDesc.SrcBlendAlpha);
			return seed;
		}
	};

	template<>
	struct hash<D3D12_SHADER_BYTECODE>
	{
		std::size_t operator()(const D3D12_SHADER_BYTECODE& obj) const {
			return std::hash<std::string>{}(std::string(static_cast<const char*>(obj.pShaderBytecode), obj.BytecodeLength));
		}
	};

	template<>
	struct hash<D3D12_DEPTH_STENCIL_DESC>
	{
		std::size_t operator()(const D3D12_DEPTH_STENCIL_DESC& obj) const {
			std::size_t seed = 0;
			hash_combine(seed, obj.BackFace);
			hash_combine(seed, obj.DepthEnable);
			hash_combine(seed, obj.DepthFunc);
			hash_combine(seed, obj.DepthWriteMask);
			hash_combine(seed, obj.FrontFace);
			hash_combine(seed, obj.StencilEnable);
			hash_combine(seed, obj.StencilReadMask);
			hash_combine(seed, obj.StencilWriteMask);
			return seed;
		}
	};

	template<>
	struct hash<D3D12_DEPTH_STENCILOP_DESC>
	{
		std::size_t operator()(const D3D12_DEPTH_STENCILOP_DESC& obj) const {
			std::size_t seed = 0;
			hash_combine(seed, obj.StencilDepthFailOp);
			hash_combine(seed, obj.StencilFailOp);
			hash_combine(seed, obj.StencilFunc);
			hash_combine(seed, obj.StencilPassOp);
			return seed;
		}
	};

	template<>
	struct hash<DXGI_FORMAT>
	{
		std::size_t operator()(const DXGI_FORMAT& obj) const {
			return std::hash<int>{}(static_cast<int>(obj));
		}
	};

	template<>
	struct  hash<DXGI_SAMPLE_DESC>
	{
		std::size_t operator()(const DXGI_SAMPLE_DESC& obj) const {
			std::size_t seed = 0;
			hash_combine(seed, obj.Count);
			hash_combine(seed, obj.Quality);
			return seed;
		}
	};

	template<>
	struct hash<D3D12_DESCRIPTOR_HEAP_TYPE>
	{
		std::size_t operator()(const D3D12_DESCRIPTOR_HEAP_TYPE& obj)const {
			return std::hash<int>{}(static_cast<int>(obj));
		}
	};

	template<>
	struct hash<DirectX::XMFLOAT3>
	{
		std::size_t operator()(const DirectX::XMFLOAT3& obj) const {
			std::size_t seed = 0;
			hash_combine(seed, obj.x);
			hash_combine(seed, obj.y);
			hash_combine(seed, obj.z);
			return seed;
		};
	};

	template<>
	struct hash<DirectX::XMFLOAT2>
	{
		std::size_t operator()(const DirectX::XMFLOAT2& obj) const {
			std::size_t seed = 0;
			hash_combine(seed, obj.x);
			hash_combine(seed, obj.y);
			return seed;
		};
	};
}
