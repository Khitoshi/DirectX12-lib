#include "CommandQueue.h"
#include <stdexcept>

/// <summary>
/// 初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
/// <param name="desc">設定</param>
void CommandQueue::init(ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC desc)
{
    if (FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&this->command_queue_)))) {
        throw std::runtime_error("failed to create command queue");
    }
}
