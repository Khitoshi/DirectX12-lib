#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;


/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐����p�N���X
/// </summary>
class PipelineStateObject
{
public:
    /// <summary>
    /// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�������Ɏg�p����ݒ�
    /// </summary>
    struct PipelineStateObjectConf {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;

        bool operator==(const PipelineStateObjectConf& other) const {
			return memcmp(this, &other, sizeof(PipelineStateObjectConf)) == 0;
		}
    };

public:
    PipelineStateObject(PipelineStateObjectConf c) :pipelineStateObject(),conf(c) {};
    ~PipelineStateObject() {};

    //����������
    void init(ID3D12Device* device);

private:
    //�O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
    void createGraphicsPipelineStateObject(ID3D12Device* device);

public:
    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̎擾
    ID3D12PipelineState* getPipelineStateObject() const { return pipelineStateObject.Get(); }

private:
    ComPtr<ID3D12PipelineState> pipelineStateObject;    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
    PipelineStateObjectConf conf;
};
