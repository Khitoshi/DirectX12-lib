#include "DescriptorHeapManager.h"

std::vector<DescriptorHeap*> DescriptorHeapManager::descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

void DescriptorHeapManager::clear(const D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	for (auto& descriptor : descriptor_heaps_[type]) {
		descriptor->clear();
	}
}

void DescriptorHeapManager::reinit(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	for (auto& descriptor : descriptor_heaps_[type]) {
		descriptor->reinit(device);
	}
}

void DescriptorHeapManager::regist(const D3D12_DESCRIPTOR_HEAP_TYPE type, DescriptorHeap* descriptor_heap)
{
	descriptor_heaps_[type].push_back(descriptor_heap);
}
