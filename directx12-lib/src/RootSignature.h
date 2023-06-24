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
    //ルートシグネチャの作成
    void createRootSignature(RootSignatureConf conf);

public:
    //ルートシグネチャの取得
    ID3D12RootSignature* getRootSignature() const { return rootSignature.Get(); }

private:
    ComPtr<ID3D12RootSignature> rootSignature;//ルートシグネチャ
};
