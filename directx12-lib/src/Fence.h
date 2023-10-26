#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// GPUと同期するオブジェクト生成クラス
/// </summary>
class Fence
{
	friend class FenceFactory;
private:
	Fence() :
		fence_(),
		value_(0),
		event_()
	{};

public:
	~Fence() {};

private://初期化系
	void init(ID3D12Device* device);
	void createFence(ID3D12Device* device);
	void createValue();
	void createEvent();

public://取得系
	ID3D12Fence* getFence() const { return this->fence_.Get(); }
	UINT64 getValue() const { return this->value_; }
	HANDLE getEvent() const { return this->event_; }

	void incrementValue() { this->value_++; }
private:
	ComPtr<ID3D12Fence> fence_;
	UINT64 value_;
	HANDLE event_;
};
