#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// �R�}���h�A���P�[�^�[
/// </summary>
class CommandAllocator
{
    friend class CommandAllocatorFactory;
private:
    CommandAllocator() :command_allocator_() {};
public:
    ~CommandAllocator() {};

    //���Z�b�g
    void reset();

private:
    //������
    void init(ID3D12Device* device);
public://�擾�n
    //�R�}���h�A���P�[�^�[�̎擾
    ID3D12CommandAllocator* GetAllocator() { return command_allocator_.Get(); }

private:
    ComPtr<ID3D12CommandAllocator> command_allocator_;  //�R�}���h�A���P�[�^
};
