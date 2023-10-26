#pragma once
#include "Fence.h"
#include <memory>

/// <summary>
/// フェンスのファクトリクラス
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
