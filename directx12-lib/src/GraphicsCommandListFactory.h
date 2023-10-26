#pragma once
#include <memory>
#include "GraphicsCommandList.h"

/// <summary>
/// グラフィックスコマンドリストファクトリクラス
/// </summary>
class GraphicsCommandListFactory
{
public:
	static std::shared_ptr<GraphicsCommandList> create(ID3D12Device* device, ID3D12CommandAllocator* command_allocator)
	{
		std::shared_ptr<GraphicsCommandList> cmd_list(new GraphicsCommandList());
		cmd_list->init(device, command_allocator);
		return cmd_list;
	}
};
