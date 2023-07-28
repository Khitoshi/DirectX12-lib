#pragma once
#include "DepthStencil.h"
#include <memory>

class DepthStencilFactory
{
public:

    /// <summary>
    /// �[�x�X�e���V�����쐬����
    /// </summary>
    /// <param name="conf">�����ɕK�v�Ȑݒ�</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// �[�x�X�e���V��
    /// </returns>
    static std::shared_ptr<DepthStencil> create(const DepthStencil::DepthStencilConf& conf, ID3D12Device* device) {
        std::shared_ptr<DepthStencil> ds(new DepthStencil(conf));
        ds->init(device);
        return ds;
    }
};
