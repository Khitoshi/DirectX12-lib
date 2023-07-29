#pragma once
#include "OffScreenRenderTarget.h"
#include <memory>

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g�̃t�@�N�g���[�N���X
/// </summary>
class OffScreenRenderTargetFactory
{
public:
    /// <summary>
    /// �I�t�X�N���[�������_�[�^�[�Q�b�g�𐶐�����
    /// </summary>
    /// <param name="conf">�����Ɋւ���ݒ�</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&�����������I�t�X�N���[�������_�[�^�[�Q�b�g�̃C���X�^���X
    /// </returns>
    static std::shared_ptr<OffScreenRenderTarget> create(const OffScreenRenderTarget::OffScreenRenderTargetConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<OffScreenRenderTarget> off_screen_render_target(new OffScreenRenderTarget(conf));
        off_screen_render_target->init(device);
        return off_screen_render_target;
    }
};
