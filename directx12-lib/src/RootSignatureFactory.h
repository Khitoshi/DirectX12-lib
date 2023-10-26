#pragma once
#include "RootSignature.h"
#include <memory>

/// <summary>
/// ���[�g�V�O�l�`���t�@�N�g���[ �N���X
/// </summary>
class RootSignatureFactory
{
public:
	static std::shared_ptr<RootSignature> create(ID3D12Device* device, const RootSignature::RootSignatureConf& conf)
	{
		std::shared_ptr<RootSignature> root_signature(new RootSignature(conf));
		root_signature->init(device);
		return root_signature;
	}
};
