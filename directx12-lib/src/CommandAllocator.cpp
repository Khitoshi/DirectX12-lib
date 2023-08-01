#include "CommandAllocator.h"
#include <stdexcept>

/// <summary>
/// �R�}���h�A���P�[�^�̃��Z�b�g
/// </summary>
void CommandAllocator::reset()
{
    if (FAILED(this->command_allocator_->Reset())) {
        throw std::runtime_error("failed to reset command allocator");
    }
}

/// <summary>
/// �R�}���h�A���P�[�^�̏�����
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void CommandAllocator::init(ID3D12Device* device)
{
    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->command_allocator_)))) {
        throw std::runtime_error("failed to create command allocator");
    }
}