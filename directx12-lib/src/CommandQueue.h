#pragma once

#include "d3dx12.h"
using namespace Microsoft::WRL;

/// <summary>
/// �R�}���h�L���[���b�s���O�N���X
/// </summary>
class CommandQueue
{
	friend class CommandQueueFactory;
private:
	CommandQueue() :command_queue_() {};
public:
	~CommandQueue() {};

private://�������n
	void init(ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC desc);

public://�擾�n

	ID3D12CommandQueue* getCommandQueue() { return command_queue_.Get(); }

private:
	ComPtr<ID3D12CommandQueue>command_queue_;
};
