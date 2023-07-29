#pragma once
#include <memory>
#include "GraphicsCommandList.h"

/// <summary>
/// �O���t�B�b�N�X�R�}���h���X�g�̐������s���N���X
/// </summary>
class GraphicsCommandListFactory
{
public:
    /// <summary>
    /// �O���t�B�b�N�X�R�}���h���X�g�𐶐�����
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <param name="command_allocator">�R�}���h�A���P�[�^</param>
    /// <returns>
    /// ����&���������ꂽ�O���t�B�b�N�X�R�}���h���X�g
    /// </returns>
    static std::shared_ptr<GraphicsCommandList> create(ID3D12Device* device, ID3D12CommandAllocator* command_allocator)
    {
        std::shared_ptr<GraphicsCommandList> cmd_list(new GraphicsCommandList());
        cmd_list->init(device, command_allocator);
        return cmd_list;
    }
};
