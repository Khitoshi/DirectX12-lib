#include "RootSignature.h"
#include <stdexcept>

/// <summary>
/// ルートシグニチャの初期化処理
/// </summary>
/// <param name="conf">ルートシグニチャ生成用設定</param>
void RootSignature::init(ID3D12Device* device, const RootSignatureConf conf)
{
    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = conf.samplerFilter;
    samplerDesc.AddressU = conf.textureAdressModeU;
    samplerDesc.AddressV = conf.textureAdressModeV;
    samplerDesc.AddressW = conf.textureAdressModeW;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.ShaderRegister = 0;
    samplerDesc.RegisterSpace = 0;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    createRootSignature(
        device,
        &samplerDesc,
        conf.maxCbvDescriptor,
        conf.maxSrvDescriptor,
        conf.maxUavDescritor,
        conf.numSampler,
        conf.offsetInDescriptorsFromTableStartCB,
        conf.offsetInDescriptorsFromTableStartSRV,
        conf.offsetInDescriptorsFromTableStartUAV,
        conf.rootSignatureFlags
    );
}

void RootSignature::createRootSignature(
    ID3D12Device* device,
    D3D12_STATIC_SAMPLER_DESC* sampler,
    const UINT maxCbvDescriptor,
    const UINT maxSrvDescriptor,
    const UINT maxUavDescritor,
    const UINT numSampler,
    const UINT offsetInDescriptorsFromTableStartCB,
    const UINT offsetInDescriptorsFromTableStartSRV,
    const UINT offsetInDescriptorsFromTableStartUAV,
    const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags
)
{
    enum {
        enDescriptorHeap_CB,    //定数バッファ
        enDescriptorHeap_SRV,   //シェーダーリソースビュー
        enDescriptorHeap_UAV,   //アンオーダードアクセスビュー

        enNumDescriptorHeap     //ディスクリプタヒープの種類の数
    };

    CD3DX12_DESCRIPTOR_RANGE1 ranges[enNumDescriptorHeap] = {};
    CD3DX12_ROOT_PARAMETER1 rootParameters[enNumDescriptorHeap] = {};

    //CBV
    ranges[enDescriptorHeap_CB].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, maxCbvDescriptor, offsetInDescriptorsFromTableStartCB);
    rootParameters[enDescriptorHeap_CB].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_CB], D3D12_SHADER_VISIBILITY_ALL);

    //SRV
    ranges[enDescriptorHeap_SRV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, maxSrvDescriptor, offsetInDescriptorsFromTableStartSRV);
    rootParameters[enDescriptorHeap_SRV].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_SRV], D3D12_SHADER_VISIBILITY_ALL);

    //UAV
    ranges[enDescriptorHeap_UAV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, maxUavDescritor, offsetInDescriptorsFromTableStartUAV);
    rootParameters[enDescriptorHeap_UAV].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_UAV], D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, numSampler, sampler, rootSignatureFlags);

    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error))) {
        throw std::runtime_error("D3DX12SerializeVersionedRootSignature failed");
    }

    //ルートシグニチャの作成
    if (FAILED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->rootSignature)))) {
        throw std::runtime_error("CreateRootSignature failed");
    }
}
