#include "RootSignature.h"
#include <stdexcept>

void RootSignature::init(RootSignatureConf conf)
{
    createRootSignature(conf);
}

/// <summary>
/// ���[�g�V�O�l�`���̍쐬
/// </summary>
/// <param name="rootSignatureConf">���[�g�V�O�l�`���ݒ�</param>
/// <returns>
/// �����������[�g�V�O�l�`��
/// </returns>
void RootSignature::createRootSignature(RootSignatureConf rootSignatureConf)
{
    //���[�g�p�����[�^�̐ݒ�
    D3D12_ROOT_PARAMETER param = {};
    param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    param.Descriptor.RegisterSpace = 0;
    param.Descriptor.ShaderRegister = 0;
    param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    D3D12_ROOT_SIGNATURE_FLAGS flag =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    //���[�g�V�O�l�`���̐ݒ�
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = 1;
    desc.pParameters = &param;
    desc.NumStaticSamplers = 0;
    desc.pStaticSamplers = nullptr;
    desc.Flags = flag;

    //�V���A���C�Y��
    ComPtr<ID3DBlob> errBlob = nullptr;
    ComPtr<ID3DBlob> pBlob = nullptr;
    if (FAILED(
        D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, pBlob.GetAddressOf(), errBlob.GetAddressOf())
    )) {
        throw std::runtime_error("failed to serialize root signature:" + std::string((char*)errBlob->GetBufferPointer()));
    }

    //���[�g�V�O�l�`���̍쐬
    if (FAILED(
        rootSignatureConf.device->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(this->rootSignature.GetAddressOf())
    ))) {
        throw std::runtime_error("failed to create root signature");
    }
}
