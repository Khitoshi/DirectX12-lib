#include "Fence.h"
#include <stdexcept>

/// <summary>
/// GPUと同期オブジェクトの初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
void Fence::init(ID3D12Device* device)
{
    createFence(device);
    createValue();
    createEvent();
}

/// <summary>
/// Fenceの生成
/// </summary>
/// <param name="device"></param>
void Fence::createFence(ID3D12Device* device)
{
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence_)))) {
        throw std::runtime_error("failed to create fence");
    }
}

/// <summary>
/// fenceの初期値の生成(設定)
/// </summary>
void Fence::createValue()
{
    this->value_ = 1;
}

/// <summary>
/// イベントの生成
/// </summary>
void Fence::createEvent()
{
    this->event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (this->event_ == nullptr) {
        throw std::runtime_error("failed to create event");
    }
}