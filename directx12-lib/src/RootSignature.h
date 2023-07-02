#pragma once
#include "d3dx12.h"
#include <vector>
#include <algorithm>
using namespace Microsoft::WRL;

/// <summary>
/// ルートシグネチャ生成時に使用する設定
/// </summary>
struct RootSignatureConf {
    D3D12_FILTER samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    D3D12_TEXTURE_ADDRESS_MODE textureAdressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    D3D12_TEXTURE_ADDRESS_MODE textureAdressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    D3D12_TEXTURE_ADDRESS_MODE textureAdressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    int numSampler = 0;
    UINT maxCbvDescriptor = 0;
    UINT maxSrvDescriptor = 0;
    UINT maxUavDescritor = 0;
    UINT offsetInDescriptorsFromTableStartCB = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    UINT offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    UINT offsetInDescriptorsFromTableStartUAV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    bool operator==(const RootSignatureConf& other) const {
        return
            samplerFilter == other.samplerFilter &&

            (int)textureAdressModeU == (int)other.textureAdressModeU &&
            (int)textureAdressModeV == (int)other.textureAdressModeV &&
            (int)textureAdressModeW == (int)other.textureAdressModeW &&

            numSampler == other.numSampler &&

            maxCbvDescriptor == other.maxCbvDescriptor &&
            maxSrvDescriptor == other.maxSrvDescriptor &&
            maxUavDescritor == other.maxUavDescritor &&

            offsetInDescriptorsFromTableStartCB == other.offsetInDescriptorsFromTableStartCB &&
            offsetInDescriptorsFromTableStartSRV == other.offsetInDescriptorsFromTableStartSRV &&
            offsetInDescriptorsFromTableStartUAV == other.offsetInDescriptorsFromTableStartUAV &&

            rootSignatureFlags == other.rootSignatureFlags;
    }
};

/// <summary>
/// ルートシグニチャの生成用クラス
/// </summary>
class RootSignature
{
public:
    RootSignature() :rootSignature() {}
    ~RootSignature() {}

    //初期化処理
    void init(ID3D12Device* device, const RootSignatureConf conf);
private:
    //ルートシグネチャの作成
    //void createRootSignature(RootSignatureConf conf);
    //ルートシグニチャ生成
    void createRootSignature(
        ID3D12Device* device,
        D3D12_STATIC_SAMPLER_DESC* sampler,
        const UINT maxCbvDescriptor = 0,
        const UINT maxSrvDescriptor = 0,
        const UINT maxUavDescritor = 0,
        const UINT numSampler = 0,
        const UINT offsetInDescriptorsFromTableStartCB = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        const UINT offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        const UINT offsetInDescriptorsFromTableStartUAV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );


public:
    //ルートシグネチャの取得
    ID3D12RootSignature* getRootSignature() const { return rootSignature.Get(); }

private:
    ComPtr<ID3D12RootSignature> rootSignature;//ルートシグネチャ
};
