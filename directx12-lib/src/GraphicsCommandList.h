#pragma once
#include "d3dx12.h"
using namespace Microsoft::WRL;

/// <summary>
/// グラフィックスコマンドリストラッピングクラス
/// </summary>
class GraphicsCommandList
{
public:
	friend class GraphicsCommandListFactory;
private:
	GraphicsCommandList() :command_list_() {};

public:
	~GraphicsCommandList() {};

private://初期化系
	void init(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator);

public://取得系
	ID3D12GraphicsCommandList5* GetCommandList() { return this->command_list_.Get(); }

private:
	ComPtr<ID3D12GraphicsCommandList5> command_list_;
};
