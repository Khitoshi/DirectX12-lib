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
void RootSignature::createRootSignature(RootSignatureConf rootSignatureConf)
{
    //ルートシグニチャの設定
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