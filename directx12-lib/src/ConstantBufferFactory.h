#pragma once

#include "ConstantBuffer.h"
#include <memory>

/// <summary>
/// �萔�o�b�t�@�̃t�@�N�g���[�N���X
/// </summary>
class ConstantBufferFactory
{
public:
    /// <summary>
    /// �����֐�
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&���������ꂽ�萔�o�b�t�@
    /// </returns>
    static std::shared_ptr<ConstantBuffer> create(ID3D12Device* device, const ConstantBuffer::ConstantBufferConf& conf)
    {
        std::shared_ptr<ConstantBuffer> cb(new ConstantBuffer(conf));
        cb->init(device);
        return cb;
    }
};
