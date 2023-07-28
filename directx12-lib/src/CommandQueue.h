#pragma once

#include "d3dx12.h"
using namespace Microsoft::WRL;

/// <summary>
/// コマンドキュー
/// </summary>
class CommandQueue
{
    friend class CommandQueueFactory;
private:
    CommandQueue() :command_queue_() {};
public:
    ~CommandQueue() {};

private:
    //初期化
    void init(ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC desc);

public://取得系
    //コマンドキューを取得
    ID3D12CommandQueue* getCommandQueue() { return command_queue_.Get(); }

private:
    ComPtr<ID3D12CommandQueue>command_queue_;   //コマンドキュー

};
