#pragma once
#include "d3dx12.h"
#include <vector>
using namespace Microsoft::WRL;

/// <summary>
/// ���[�g�V�O�l�`���������Ɏg�p����ݒ�
/// </summary>
struct RootSignatureConf {
    ID3D12Device* device;
    std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplerDescArray;
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags;
};

/// <summary>
/// ���[�g�V�O�j�`���̐����p�N���X
/// </summary>
class RootSignature
{
public:
    RootSignature() :rootSignature() {}
    ~RootSignature() {}

    //����������
    void init(RootSignatureConf conf);

private:
    //���[�g�V�O�l�`���̍쐬
    //void createRootSignature(RootSignatureConf conf);
    //���[�g�V�O�j�`������
    void createRootSignature(RootSignatureConf conf);
public:
    //���[�g�V�O�l�`���̎擾
    ID3D12RootSignature* getRootSignature() const { return rootSignature.Get(); }

private:
    ComPtr<ID3D12RootSignature> rootSignature;//���[�g�V�O�l�`��
};
