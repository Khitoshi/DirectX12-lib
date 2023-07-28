#pragma once
#include "CommandAllocator.h"
#include <memory>

/// <summary>
/// �R�}���h�A���P�[�^�̐������s���N���X
/// </summary>
class CommandAllocatorFactory
{
public:
    /// <summary>
    /// �R�}���h�A���P�[�^�̐���
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&���������ꂽ�R�}���h�A���P�[�^
    /// </returns>
    static std::shared_ptr<CommandAllocator> create(ID3D12Device* device)
    {
        std::shared_ptr<CommandAllocator> command_allocator(new CommandAllocator());
        command_allocator->init(device);
        return command_allocator;
    }
};
