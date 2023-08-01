#pragma once
#include "Fence.h"
#include <memory>

/// <summary>
/// �t�F���X�̐������s���N���X
/// </summary>
class FenceFactory
{
public:
    /// <summary>
    /// GPU��CPU�̓�������邽�߂�Fence���쐬����
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&���������ꂽFence
    /// </returns>
    static std::shared_ptr<Fence> create(ID3D12Device* device)
    {
        std::shared_ptr<Fence> fence(new Fence());
        fence->init(device);
        return fence;
    }
};
