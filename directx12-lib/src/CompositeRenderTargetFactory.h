#pragma once
#include <memory>
#include "CompositeRenderTarget.h"

class CompositeRenderTargetFactory
{
public:
    /// <summary>
    /// �R���|�W�b�g�����_�[�^�[�Q�b�g�𐶐�����
    /// </summary>
    /// <param name="crt_conf">�����ɕK�v�Ȑݒ�</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&�����������R���|�W�b�g�����_�[�^�[�Q�b�g
    /// </returns>
    static std::shared_ptr<CompositeRenderTarget> create(const CompositeRenderTarget::CompositeRenderTargetConf& crt_conf, ID3D12Device* device)
    {
        std::shared_ptr<CompositeRenderTarget> compositeRenderTarget(new CompositeRenderTarget(crt_conf));
        compositeRenderTarget->init(device);
        return compositeRenderTarget;
    }
};
