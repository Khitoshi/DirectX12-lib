#pragma once
#include "CommandAllocator.h"
#include <memory>

/// <summary>
/// コマンドアロケータファクトリクラス
/// </summary>
class CommandAllocatorFactory
{
public:
	static std::shared_ptr<CommandAllocator> create(ID3D12Device* device)
	{
		std::shared_ptr<CommandAllocator> command_allocator(new CommandAllocator());
		command_allocator->init(device);
		return command_allocator;
	}
};
