#include "Fence.h"
#include <stdexcept>


void Fence::init(ID3D12Device* device)
{
	createFence(device);
	createValue();
	createEvent();
}

void Fence::createFence(ID3D12Device* device)
{
	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence_)))) {
		throw std::runtime_error("failed to create fence");
	}
}

void Fence::createValue()
{
	this->value_ = 1;
}

void Fence::createEvent()
{
	this->event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (this->event_ == nullptr) {
		throw std::runtime_error("failed to create event");
	}
}