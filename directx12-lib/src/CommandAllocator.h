#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// コマンドアロケーターラッピングクラス
/// </summary>
class CommandAllocator
{
	friend class CommandAllocatorFactory;
private:
	CommandAllocator() :command_allocator_() {};
public:
	~CommandAllocator() {};

	void reset();

private://初期化系
	void init(ID3D12Device* device);

public://取得系
	ID3D12CommandAllocator* GetAllocator() { return command_allocator_.Get(); }

private:
	ComPtr<ID3D12CommandAllocator> command_allocator_;
};
