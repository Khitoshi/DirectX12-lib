#pragma once
#include "CommandQueue.h"

/// <summary>
/// �R�}���h�L���[�����N���X
/// </summary>
class CommandQueueFactory
{
public:
    /// <summary>
    /// �R�}���h�L���[�𐶐�����
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <param name="desc">�R�}���h�L���[�̐ݒ�</param>
    /// <returns>
    /// ����&���������ꂽ�R�}���h�L���[
    /// </returns>
    static std::shared_ptr<CommandQueue> create(ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC& desc) {
        std::shared_ptr<CommandQueue> command_queue(new CommandQueue());
        command_queue->init(device, desc);
        return command_queue;
    }
};
