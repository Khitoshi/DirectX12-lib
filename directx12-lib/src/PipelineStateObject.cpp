#include "PipelineStateObject.h"
#include <stdexcept>

/// <summary>
/// ����������
/// </summary>
/// <param name="device">�f�o�C�X</param>
/// <param name="psoDesc">pso�̐ݒ�</param>
void PipelineStateObject::init(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
	this->pipelineStateObject = createGraphicsPipelineStateObject(device, psoDesc);
}

/// <summary>
/// �O���t�B�b�N�X�p�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
/// </summary>
/// <param name="device">�f�o�C�X</param>
/// <param name="psoDesc">pso�̐ݒ�</param>
/// <returns>
/// ���������p�C�v���C���X�e�[�g�I�u�W�F�N�g
/// </returns>
ComPtr<ID3D12PipelineState> PipelineStateObject::createGraphicsPipelineStateObject(ID3D12Device* device,D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
	ComPtr<ID3D12PipelineState> pso = nullptr;
	if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)))) {
		throw std::runtime_error("failed to create pipeline state object");
	}
	return pso;
}
