#pragma once
#include <memory>
#include "GraphicsCommandList.h"

/// <summary>
/// グラフィックスコマンドリストの生成を行うクラス
/// </summary>
class GraphicsCommandListFactory
{
public:
    /// <summary>
    /// グラフィックスコマンドリストを生成する
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="command_allocator">コマンドアロケータ</param>
    /// <returns>
    /// 生成&初期化されたグラフィックスコマンドリスト
    /// </returns>
    static std::shared_ptr<GraphicsCommandList> create(ID3D12Device* device, ID3D12CommandAllocator* command_allocator)
    {
        std::shared_ptr<GraphicsCommandList> cmd_list(new GraphicsCommandList());
        cmd_list->init(device, command_allocator);
        return cmd_list;
    }
};
