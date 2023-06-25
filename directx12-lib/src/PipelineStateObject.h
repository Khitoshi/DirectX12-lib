#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�������Ɏg�p����ݒ�
/// </summary>
struct PipelineStateObjectConf {
    ID3D12Device* device;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
};

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐����p�N���X
/// </summary>
class PipelineStateObject
{
public:
    PipelineStateObject() :pipelineStateObject() {};
    ~PipelineStateObject() {};

    //����������
    void init(PipelineStateObjectConf conf);

private:
    //�O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
    void createGraphicsPipelineStateObject(PipelineStateObjectConf conf);

public:
    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̎擾
    ID3D12PipelineState* getPipelineStateObject() const { return pipelineStateObject.Get(); }

private:
    ComPtr<ID3D12PipelineState> pipelineStateObject;    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
};
