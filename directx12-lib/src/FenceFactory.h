#pragma once
#include "Fence.h"
#include <memory>

/// <summary>
/// フェンスの生成を行うクラス
/// </summary>
class FenceFactory
{
public:
    /// <summary>
    /// GPUとCPUの同期を取るためのFenceを作成する
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化されたFence
    /// </returns>
    static std::shared_ptr<Fence> create(ID3D12Device* device)
    {
        std::shared_ptr<Fence> fence(new Fence());
        fence->init(device);
        return fence;
    }
};
