#pragma once

#include "DescriptorHeap.h"
#include <vector>
#include <memory>


//TODO:使用していないので削除する
class CBVManager
{
private:
	CBVManager() {}
	~CBVManager() {}

public:
	static CBVManager* GetInstance()
	{
		static CBVManager instance;
		return &instance;
	}

	//登録するただし、すでに同じ内容が登録されている場合は何もしない
	void registerHeap(std::shared_ptr<DescriptorHeap> heap)
	{
		auto itr = std::find(cbv_heaps_.begin(), cbv_heaps_.end(), heap);
		if (itr != cbv_heaps_.end()) return;
		cbv_heaps_.push_back(heap);
	}

	//画面サイズを変更する際などに一度クリアする必要がある
	void clear()
	{
		for (auto& heap : cbv_heaps_) {
			heap->clear();
		}
	}

	//フルスクリーンに切り替える場合などに一度クリアしてからディスクリプタの再作成を行う必要がある
	void reInit(ID3D12Device* device)
	{
		for (auto& heap : cbv_heaps_) {
			heap->reInit(device);
		}
	}

private:
	std::vector<std::shared_ptr<DescriptorHeap>> cbv_heaps_;

};
