#pragma once
#include "Fence.h"
#include <memory>

/// <summary>
/// �t�F���X�̃t�@�N�g���N���X
/// </summary>
class FenceFactory
{
public:
	static std::shared_ptr<Fence> create(ID3D12Device* device)
	{
		std::shared_ptr<Fence> fence(new Fence());
		fence->init(device);
		return fence;
	}
};
