#include "RootSignature.h"
#include <stdexcept>

/// <summary>
/// ルートシグニチャの初期化処理
/// </summary>
/// <param name="conf">ルートシグニチャ生成用設定</param>
void RootSignature::init(ID3D12Device* device)
{
    D3D12_STATIC_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter = this->conf_.sampler_filter;
    sampler_desc.AddressU = this->conf_.texture_address_modeU;
    sampler_desc.AddressV = this->conf_.texture_address_modeV;
    sampler_desc.AddressW = this->conf_.texture_address_modeW;
    sampler_desc.MipLODBias = 0;
    sampler_desc.MaxAnisotropy = 0;
    sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    sampler_desc.MinLOD = 0.0f;
    sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
    sampler_desc.ShaderRegister = 0;
    sampler_desc.RegisterSpace = 0;
    sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    createRootSignature(
        device,
        &sampler_desc
    );
}

/// <summary>
/// ルートシグネチャの作成
/// </summary>
/// <param name="device">GPUデバイス</param>
/// <param name="sampler">サンプラ</param>
/// <param name="max_cbv_descriptor">cbvのディスクリプタ数</param>
/// <param name="max_srv_descriptor">srvのディスクリプタ数</param>
/// <param name="max_uav_descriptor">uavのディスクリプタ数</param>
/// <param name="num_sampler">サンプラ数</param>
/// <param name="offset_in_descriptors_from_table_start_cb">cbのレジスタスタート位置</param>
/// <param name="offset_in_descriptors_from_table_start_srv">srvのレジスタスタート位置</param>
/// <param name="offset_in_descriptors_from_table_start_uav">uvaのレジスタスタート位置</param>
/// <param name="rootSignatureFlags"></param>
void RootSignature::createRootSignature(
    ID3D12Device* device,
    D3D12_STATIC_SAMPLER_DESC* sampler
)
{
    std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters = {};

    //CBV
    if (this->conf_.max_cbv_descriptor > 0) {
        CD3DX12_DESCRIPTOR_RANGE1 ranges = {};
        CD3DX12_ROOT_PARAMETER1 param = {};
        ranges.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, this->conf_.max_cbv_descriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, this->conf_.offset_in_descriptors_from_table_start_cb);
        param.InitAsDescriptorTable(1, &ranges, this->conf_.visibility_cbv);
        rootParameters.push_back(param);
    }

    //SRV
    if (this->conf_.max_srv_descriptor > 0) {
        CD3DX12_DESCRIPTOR_RANGE1 ranges = {};
        CD3DX12_ROOT_PARAMETER1 param = {};
        ranges.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, this->conf_.max_srv_descriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, this->conf_.offset_in_descriptors_from_table_start_srv);
        param.InitAsDescriptorTable(1, &ranges, this->conf_.visibility_srv);
        rootParameters.push_back(param);
    }

    //UAV
    if (this->conf_.max_uav_descriptor > 0) {
        CD3DX12_DESCRIPTOR_RANGE1 ranges = {};
        CD3DX12_ROOT_PARAMETER1 param = {};
        ranges.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, this->conf_.max_uav_descriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, this->conf_.offset_in_descriptors_from_table_start_uav);
        param.InitAsDescriptorTable(1, &ranges, this->conf_.visibility_uav);
        rootParameters.push_back(param);
    }



    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data(), this->conf_.num_sampler, sampler, this->conf_.root_signature_flags);

    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &error))) {
        if (error) {
            OutputDebugStringA((char*)error->GetBufferPointer());
        }
        throw std::runtime_error("D3DX12SerializeVersionedRootSignature failed");
    }

    //ルートシグニチャの作成
    if (FAILED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->root_signature_)))) {
        throw std::runtime_error("CreateRootSignature failed");
    }
}