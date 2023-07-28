#pragma once
#include "RenderTarget.h"
#include <memory>

/// <summary>
/// �����_�[�^�[�Q�b�g�𐶐�����N���X
/// </summary>
class RenderTargetFactory
{
public:
    /// <summary>
    /// �����_�[�^�[�Q�b�g�𐶐�����
    /// </summary>
    /// <param name="conf">�����ɕK�v�ȏ��</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&���������������_�[�^�[�Q�b�g
    /// </returns>
    static std::shared_ptr<RenderTarget> create(const RenderTarget::RenderTargetConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<RenderTarget> renderTarget(new RenderTarget(conf));
        renderTarget->init(device);
        return renderTarget;
    }

};
