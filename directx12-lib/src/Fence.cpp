#include "Fence.h"
#include <stdexcept>

/// <summary>
/// GPUと同期オブジェクトの初期化
/// </summary>
/// <param name="device">デバイスインターフェース</param>
void Fence::init(ID3D12Device* device)
{
    fence = createFence(device);
    value = createValue();
    event = createEvent();
}

/// <summary>
/// Fenceの生成
/// </summary>
/// <param name="device"></param>
/// <returns>
/// GPUと同期オブジェクト
/// </returns>
ComPtr<ID3D12Fence> Fence::createFence(ID3D12Device* device)
{
    ComPtr<ID3D12Fence> f;
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&f)))) {
        throw std::runtime_error("failed to create fence");
    }

    return f;
}

/// <summary>
/// fenceの初期値の生成(設定)
/// </summary>
/// <returns>
/// 設定した初期値
/// </returns>
UINT64 Fence::createValue()
{
    UINT value = 1;
    return value;
}

/// <summary>
/// イベントの生成
/// </summary>
/// <returns>
/// 生成したイベント
/// </returns>
HANDLE Fence::createEvent()
{
    HANDLE e = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (e == nullptr) {
        throw std::runtime_error("failed to create event");
    }

    return e;
}