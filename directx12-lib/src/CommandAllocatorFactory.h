#pragma once
#include "CommandAllocator.h"
#include <memory>

/// <summary>
/// コマンドアロケータの生成を行うクラス
/// </summary>
class CommandAllocatorFactory
{
public:
    /// <summary>
    /// コマンドアロケータの生成
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化されたコマンドアロケータ
    /// </returns>
    static std::shared_ptr<CommandAllocator> create(ID3D12Device* device)
    {
        std::shared_ptr<CommandAllocator> command_allocator(new CommandAllocator());
        command_allocator->init(device);
        return command_allocator;
    }
};
