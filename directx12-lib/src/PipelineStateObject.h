#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐����p�N���X
/// </summary>
class PipelineStateObject
{
    friend class PipelineStateObjectFactory;
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

private:
    PipelineStateObject(const PipelineStateObjectConf& c) :
        conf_(c),
        pso_()
    {};

public:
    ~PipelineStateObject() {};

private:
    //����������
    void init(ID3D12Device* device);

public:
    //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̎擾
    ID3D12PipelineState* getPipelineStateObject() const { return pso_.Get(); }

private:
    PipelineStateObjectConf conf_;      //�p�C�v���C���X�e�[�g�I�u�W�F�N�g�������Ɏg�p����ݒ�
    ComPtr<ID3D12PipelineState> pso_;   //�p�C�v���C���X�e�[�g�I�u�W�F�N�g
};
