#pragma once
#include "d3dx12.h"

#include <vector>
#include <algorithm>

using namespace Microsoft::WRL;

/// <summary>
/// ルートシグニチャの生成用クラス
/// </summary>
class RootSignature
{
	friend class RootSignatureFactory;
public:
	struct RootSignatureConf {
		D3D12_FILTER sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		D3D12_TEXTURE_ADDRESS_MODE texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		D3D12_TEXTURE_ADDRESS_MODE texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		D3D12_TEXTURE_ADDRESS_MODE texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		int num_sampler = 0;
		UINT num_cbv = 0;
		UINT num_srv = 0;
		UINT num_uav = 0;
		D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		D3D12_SHADER_VISIBILITY visibility_cbv = D3D12_SHADER_VISIBILITY_ALL;
		D3D12_SHADER_VISIBILITY visibility_srv = D3D12_SHADER_VISIBILITY_ALL;
		D3D12_SHADER_VISIBILITY visibility_uav = D3D12_SHADER_VISIBILITY_ALL;

		bool operator==(const RootSignatureConf& other) const {
			return
				sampler_filter == other.sampler_filter &&

				(int)texture_address_modeU == (int)other.texture_address_modeU &&
				(int)texture_address_modeV == (int)other.texture_address_modeV &&
				(int)texture_address_modeW == (int)other.texture_address_modeW &&

				num_sampler == other.num_sampler &&

				num_cbv == other.num_cbv &&
				num_srv == other.num_srv &&
				num_uav == other.num_uav &&

				root_signature_flags == other.root_signature_flags &&

				visibility_cbv == other.visibility_cbv &&
				visibility_srv == other.visibility_srv &&
				visibility_uav == other.visibility_uav;
		}
	};

private:
	RootSignature(const RootSignatureConf c) :conf_(c), root_signature_() {}

public:
	~RootSignature() {}

private:
	void init(ID3D12Device* device);
	void createRootSignature(
		ID3D12Device* device,
		D3D12_STATIC_SAMPLER_DESC* sampler
	);

public:
	ID3D12RootSignature* getRootSignature() const { return this->root_signature_.Get(); }

private:
	RootSignatureConf conf_;
	ComPtr<ID3D12RootSignature> root_signature_;
};
