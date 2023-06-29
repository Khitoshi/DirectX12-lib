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
void RootSignature::createRootSignature(RootSignatureConf conf)
{
    //���[�g�V�O�j�`���̐ݒ�
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(conf.rootParameters.size(), conf.rootParameters.data(),
        conf.samplerDescArray.size(), conf.samplerDescArray.data(),
        conf.rootSignatureFlags);

    //�V���A���C�Y
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signature, &error))) {
        throw std::runtime_error("D3DX12SerializeVersionedRootSignature failed");
    }

    //���[�g�V�O�j�`���̍쐬
    if (FAILED(conf.device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->rootSignature)))) {
		throw std::runtime_error("CreateRootSignature failed");
	}

}
