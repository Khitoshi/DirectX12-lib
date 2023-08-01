#pragma once
#include "CommandQueue.h"

/// <summary>
/// コマンドキュー生成クラス
/// </summary>
class CommandQueueFactory
{
public:
    /// <summary>
    /// コマンドキューを生成する
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="desc">コマンドキューの設定</param>
    /// <returns>
    /// 生成&初期化されたコマンドキュー
    /// </returns>
    static std::shared_ptr<CommandQueue> create(ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC& desc) {
        std::shared_ptr<CommandQueue> command_queue(new CommandQueue());
        command_queue->init(device, desc);
        return command_queue;
    }
};
