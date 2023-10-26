#pragma once
#include "CommandQueue.h"

/// <summary>
/// コマンドキューファクトリクラス
/// </summary>
class CommandQueueFactory
{
public:
	static std::shared_ptr<CommandQueue> create(ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC& desc) {
		std::shared_ptr<CommandQueue> command_queue(new CommandQueue());
		command_queue->init(device, desc);
		return command_queue;
	}
};
