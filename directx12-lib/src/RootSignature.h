#pragma once
#include "d3dx12.h"
#include <vector>
using namespace Microsoft::WRL;

/// <summary>
/// ルートシグネチャ生成時に使用する設定
/// </summary>
struct RootSignatureConf {
    ID3D12Device* device;
    std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplerDescArray;
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags;
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
    void init(RootSignatureConf conf);

private:
    //ルートシグネチャの作成
    //void createRootSignature(RootSignatureConf conf);
    //ルートシグニチャ生成
    void createRootSignature(RootSignatureConf conf);
public:
    //ルートシグネチャの取得
    ID3D12RootSignature* getRootSignature() const { return rootSignature.Get(); }

private:
    ComPtr<ID3D12RootSignature> rootSignature;//ルートシグネチャ
};
