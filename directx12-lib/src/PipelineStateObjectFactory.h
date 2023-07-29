#pragma once

#include "PipelineStateObject.h"
#include <memory>

/// <summary>
/// pso���쐬����t�@�N�g���N���X
/// </summary>
class PipelineStateObjectFactory
{
public:
    /// <summary>
    /// pso���쐬����
    /// </summary>
    /// <param name="conf">�����Ɋւ���ݒ�</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&�������ς݂�pso
    /// </returns>
    static std::shared_ptr<PipelineStateObject> create(const PipelineStateObject::PipelineStateObjectConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<PipelineStateObject> pso(new PipelineStateObject(conf));
        pso->init(device);
        return pso;
    }
};
