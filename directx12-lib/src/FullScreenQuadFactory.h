#pragma once
#include "FullScreenQuad.h"
#include <memory>

/// <summary>
/// �t���X�N���[���l�p�`�̐������s���N���X
/// </summary>
class FullScreenQuadFactory
{
public:
    /// <summary>
    /// �t���X�N���[���l�p�`�𐶐�����
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&���������ꂽ�t���X�N���[���l�p�`
    /// </returns>;
    static std::shared_ptr<FullScreenQuad> create(ID3D12Device* device)
    {
        std::shared_ptr<FullScreenQuad> quad(new FullScreenQuad());
        quad->init(device);
        return quad;
    }
};
