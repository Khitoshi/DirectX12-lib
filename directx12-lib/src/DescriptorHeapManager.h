#pragma once

#include "d3dx12.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include "DescriptorHeap.h"
#include "Hashes.h"

/// <summary>
/// ディスクリプタヒープ管理クラス
/// </summary>
class DescriptorHeapManager
{
	friend class DescriptorHeap;
private:
	DescriptorHeapManager() {}
	~DescriptorHeapManager() {}

public:
	static void clear(const D3D12_DESCRIPTOR_HEAP_TYPE type);
	static void reinit(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_TYPE type);

private:
	static void regist(const D3D12_DESCRIPTOR_HEAP_TYPE type, DescriptorHeap* descriptor_heap);

private:
	static std::vector<DescriptorHeap*> descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

};
