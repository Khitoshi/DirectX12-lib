#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

struct RootSignatureConf {
    ID3D12Device* device;
    //UINT maxCbvDescriptor;
    //UINT maxSrvDescriptor;
    //UINT maxUavDescriptor;
    //UINT offsetInDescriptorsFromTableStartCB	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    //UINT offsetInDescriptorsFromTableStartSRV	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    //UINT offsetInDescriptorsFromTableStartUAV	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    //UINT maxSamplerDescriptor;
    //D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
};

class RootSignature
{
public:
    RootSignature() :rootSignature() {}
    ~RootSignature() {}

    void init(RootSignatureConf conf);
private:
    //���[�g�V�O�l�`���̍쐬
    void createRootSignature(RootSignatureConf conf);

public:
    //���[�g�V�O�l�`���̎擾
    ID3D12RootSignature* getRootSignature() const { return rootSignature.Get(); }

private:
    ComPtr<ID3D12RootSignature> rootSignature;//���[�g�V�O�l�`��
};
