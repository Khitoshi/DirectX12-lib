#pragma once
#include "d3dx12.h"
using namespace Microsoft::WRL;

/// <summary>
/// �O���t�B�b�N�X�R�}���h���X�g���b�s���O�N���X
/// </summary>
class GraphicsCommandList
{
public:
	friend class GraphicsCommandListFactory;
private:
	GraphicsCommandList() :command_list_() {};

public:
	~GraphicsCommandList() {};

private://�������n
	void init(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator);

public://�擾�n
	ID3D12GraphicsCommandList5* GetCommandList() { return this->command_list_.Get(); }

private:
	ComPtr<ID3D12GraphicsCommandList5> command_list_;
};
