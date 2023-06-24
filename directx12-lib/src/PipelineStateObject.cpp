#include "PipelineStateObject.h"
#include <stdexcept>

/// <summary>
/// ����������
/// </summary>
/// <param name="device">�f�o�C�X</param>
/// <param name="psoDesc">pso�̐ݒ�</param>
void PipelineStateObject::init(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
    createGraphicsPipelineStateObject(device, psoDesc);
}

/// <summary>
/// �O���t�B�b�N�X�p�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
/// </summary>
/// <param name="device">�f�o�C�X</param>
/// <param name="psoDesc">pso�̐ݒ�</param>
/// <returns>
/// ���������p�C�v���C���X�e�[�g�I�u�W�F�N�g
/// </returns>
void PipelineStateObject::createGraphicsPipelineStateObject(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
    if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&this->pipelineStateObject)))) {
        throw std::runtime_error("failed to create pipeline state object");
    }
    return;
}
