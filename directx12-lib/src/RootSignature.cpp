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
    samplerDesc.AddressU = conf.textureAddressModeU;
    samplerDesc.AddressV = conf.textureAddressModeV;
    samplerDesc.AddressW = conf.textureAddressModeW;
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
        conf.maxUavDescriptor,
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
    const UINT maxUavDescriptor,
    const UINT numSampler,
    const UINT offsetInDescriptorsFromTableStartCB,
    const UINT offsetInDescriptorsFromTableStartSRV,
    const UINT offsetInDescriptorsFromTableStartUAV,
    const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags
)
{
    std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges = {};
    std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters = {};

    //CBV
    if (maxCbvDescriptor > 0) {
        ranges.emplace_back();
        rootParameters.emplace_back();
        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, maxCbvDescriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offsetInDescriptorsFromTableStartCB);

        rootParameters.back().InitAsDescriptorTable(1, &ranges.back(), D3D12_SHADER_VISIBILITY_ALL);
        //rootParameters.back().InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
    }

    //SRV
    if (maxSrvDescriptor > 0) {
        ranges.emplace_back();
        rootParameters.emplace_back();  
        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, maxSrvDescriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offsetInDescriptorsFromTableStartSRV);
        rootParameters.back().InitAsDescriptorTable(1, &ranges.back(), D3D12_SHADER_VISIBILITY_ALL);
    }

    //UAV
    if (maxUavDescriptor > 0) {
        ranges.emplace_back();
        rootParameters.emplace_back();
        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, maxUavDescriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offsetInDescriptorsFromTableStartUAV);
        rootParameters.back().InitAsDescriptorTable(1, &ranges.back(), D3D12_SHADER_VISIBILITY_ALL);
    }

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init_1_1(rootParameters.size(), rootParameters.data(), numSampler, sampler, rootSignatureFlags);

    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error))) {
        if (error) {
            OutputDebugStringA((char*)error->GetBufferPointer());
        }
        throw std::runtime_error("D3DX12SerializeVersionedRootSignature failed");
    }

    //ルートシグニチャの作成
    if (FAILED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->rootSignature)))) {
        throw std::runtime_error("CreateRootSignature failed");
    }
}
