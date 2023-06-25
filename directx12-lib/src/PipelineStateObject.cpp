#include "PipelineStateObject.h"
#include <stdexcept>

/// <summary>
/// ����������
/// </summary>
/// <param name="device">�f�o�C�X</param>
/// <param name="psoDesc">pso�̐ݒ�</param>
void PipelineStateObject::init(PipelineStateObjectConf conf)
{
    createGraphicsPipelineStateObject(conf);
}

/// <summary>
/// �O���t�B�b�N�X�p�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
/// </summary>
/// <param name="device">�f�o�C�X</param>
/// <param name="psoDesc">pso�̐ݒ�</param>
/// <returns>
/// ���������p�C�v���C���X�e�[�g�I�u�W�F�N�g
/// </returns>
void PipelineStateObject::createGraphicsPipelineStateObject(PipelineStateObjectConf conf)
{
    if (FAILED(conf.device->CreateGraphicsPipelineState(&conf.desc, IID_PPV_ARGS(&this->pipelineStateObject)))) {
        throw std::runtime_error("failed to create pipeline state object");
    }
    return;
}
