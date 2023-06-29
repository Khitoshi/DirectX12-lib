#include "RootSignature.h"
#include <stdexcept>

/// <summary>
/// ルートシグニチャの初期化処理
/// </summary>
/// <param name="conf">ルートシグニチャ生成用設定</param>
void RootSignature::init(RootSignatureConf conf)
{
    createRootSignature(conf);
}

/// <summary>
/// ルートシグニチャの作成
/// </summary>
/// <param name="rootSignatureConf">ルートシグニチャ設定</param>
/// <returns>
/// 生成したルートシグニチャ
/// </returns>   
void RootSignature::createRootSignature(RootSignatureConf conf)
{
    //ルートシグニチャの設定
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(conf.rootParameters.size(), conf.rootParameters.data(),
        conf.samplerDescArray.size(), conf.samplerDescArray.data(),
        conf.rootSignatureFlags);

    //シリアライズ
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signature, &error))) {
        throw std::runtime_error("D3DX12SerializeVersionedRootSignature failed");
    }

    //ルートシグニチャの作成
    if (FAILED(conf.device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->rootSignature)))) {
		throw std::runtime_error("CreateRootSignature failed");
	}

}
