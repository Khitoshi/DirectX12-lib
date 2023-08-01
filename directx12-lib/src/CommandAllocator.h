#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// コマンドアロケーター
/// </summary>
class CommandAllocator
{
    friend class CommandAllocatorFactory;
private:
    CommandAllocator() :command_allocator_() {};
public:
    ~CommandAllocator() {};

    //リセット
    void reset();

private:
    //初期化
    void init(ID3D12Device* device);
public://取得系
    //コマンドアロケーターの取得
    ID3D12CommandAllocator* GetAllocator() { return command_allocator_.Get(); }

private:
    ComPtr<ID3D12CommandAllocator> command_allocator_;  //コマンドアロケータ
};
