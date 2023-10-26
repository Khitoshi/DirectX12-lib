#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// �R�}���h�A���P�[�^�[���b�s���O�N���X
/// </summary>
class CommandAllocator
{
	friend class CommandAllocatorFactory;
private:
	CommandAllocator() :command_allocator_() {};
public:
	~CommandAllocator() {};

	void reset();

private://�������n
	void init(ID3D12Device* device);

public://�擾�n
	ID3D12CommandAllocator* GetAllocator() { return command_allocator_.Get(); }

private:
	ComPtr<ID3D12CommandAllocator> command_allocator_;
};
