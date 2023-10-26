#pragma once
#include "FullScreenQuad.h"
#include <memory>

/// <summary>
/// �t���X�N���[���l�p�`�t�@�N�g���N���X
/// </summary>
class FullScreenQuadFactory
{
public:
	static std::shared_ptr<FullScreenQuad> create(ID3D12Device* device)
	{
		std::shared_ptr<FullScreenQuad> quad(new FullScreenQuad());
		quad->init(device);
		return quad;
	}
};
