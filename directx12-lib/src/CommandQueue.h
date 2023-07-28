#pragma once

#include "d3dx12.h"
using namespace Microsoft::WRL;

/// <summary>
/// �R�}���h�L���[
/// </summary>
class CommandQueue
{
    friend class CommandQueueFactory;
private:
    CommandQueue() :command_queue_() {};
public:
    ~CommandQueue() {};

private:
    //������
    void init(ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC desc);

public://�擾�n
    //�R�}���h�L���[���擾
    ID3D12CommandQueue* getCommandQueue() { return command_queue_.Get(); }

private:
    ComPtr<ID3D12CommandQueue>command_queue_;   //�R�}���h�L���[

};
