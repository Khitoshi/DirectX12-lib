#include "RootSignature.h"
#include <stdexcept>

/// <summary>
/// ���[�g�V�O�j�`���̏���������
/// </summary>
/// <param name="conf">���[�g�V�O�j�`�������p�ݒ�</param>
void RootSignature::init(RootSignatureConf conf)
{
    createRootSignature(conf);
}

/// <summary>
/// ���[�g�V�O�j�`���̍쐬
/// </summary>
/// <param name="rootSignatureConf">���[�g�V�O�j�`���ݒ�</param>
/// <returns>
/// �����������[�g�V�O�j�`��
/// </returns>
void RootSignature::createRootSignature(RootSignatureConf rootSignatureConf)
{
    //���[�g�V�O�j�`���̐ݒ�
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 0;
    rootSignatureDesc.pParameters = nullptr;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> serializeRootSignature = nullptr;
    ComPtr<ID3DBlob> errBlob = nullptr;
    if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializeRootSignature, &errBlob))) {
        throw std::runtime_error("D3D12SerializeRootSignature failed");
        if (errBlob) {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
        }
    }

    if (FAILED(rootSignatureConf.device->CreateRootSignature(0, serializeRootSignature->GetBufferPointer(), serializeRootSignature->GetBufferSize(), IID_PPV_ARGS(&this->rootSignature)))) {
        throw std::runtime_error("CreateRootSignature failed");
    }
}