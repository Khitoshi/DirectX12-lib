#include "RootSignature.h"
#include <stdexcept>

/// <summary>
/// ���[�g�V�O�j�`���̏���������
/// </summary>
/// <param name="conf">���[�g�V�O�j�`�������p�ݒ�</param>
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
        &sampler_desc,
        this->conf_.max_cbv_descriptor,
        this->conf_.max_srv_descriptor,
        this->conf_.max_uav_descriptor,
        this->conf_.num_sampler,
        this->conf_.offset_in_descriptors_from_table_start_cb,
        this->conf_.offset_in_descriptors_from_table_start_srv,
        this->conf_.offset_in_descriptors_from_table_start_uav,
        this->conf_.root_signature_flags
    );
}

/// <summary>
/// ���[�g�V�O�l�`���̍쐬
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
/// <param name="sampler">�T���v��</param>
/// <param name="max_cbv_descriptor">cbv�̃f�B�X�N���v�^��</param>
/// <param name="max_srv_descriptor">srv�̃f�B�X�N���v�^��</param>
/// <param name="max_uav_descriptor">uav�̃f�B�X�N���v�^��</param>
/// <param name="num_sampler">�T���v����</param>
/// <param name="offset_in_descriptors_from_table_start_cb">cb�̃��W�X�^�X�^�[�g�ʒu</param>
/// <param name="offset_in_descriptors_from_table_start_srv">srv�̃��W�X�^�X�^�[�g�ʒu</param>
/// <param name="offset_in_descriptors_from_table_start_uav">uva�̃��W�X�^�X�^�[�g�ʒu</param>
/// <param name="rootSignatureFlags"></param>
void RootSignature::createRootSignature(
    ID3D12Device* device,
    D3D12_STATIC_SAMPLER_DESC* sampler,
    const UINT max_cbv_descriptor,
    const UINT max_srv_descriptor,
    const UINT max_uav_descriptor,
    const UINT num_sampler,
    const UINT offset_in_descriptors_from_table_start_cb,
    const UINT offset_in_descriptors_from_table_start_srv,
    const UINT offset_in_descriptors_from_table_start_uav,
    const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags
)
{
    std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges = {};
    std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters = {};

    //CBV
    if (max_cbv_descriptor > 0) {
        ranges.emplace_back();
        rootParameters.emplace_back();
        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, max_cbv_descriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offset_in_descriptors_from_table_start_cb);

        rootParameters.back().InitAsDescriptorTable(1, &ranges.back(), D3D12_SHADER_VISIBILITY_ALL);
    }

    //SRV
    if (max_srv_descriptor > 0) {
        ranges.emplace_back();
        rootParameters.emplace_back();
        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, max_srv_descriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offset_in_descriptors_from_table_start_srv);
        rootParameters.back().InitAsDescriptorTable(1, &ranges.back(), D3D12_SHADER_VISIBILITY_ALL);
    }

    //UAV
    if (max_uav_descriptor > 0) {
        ranges.emplace_back();
        rootParameters.emplace_back();
        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, max_uav_descriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offset_in_descriptors_from_table_start_uav);
        rootParameters.back().InitAsDescriptorTable(1, &ranges.back(), D3D12_SHADER_VISIBILITY_ALL);
    }

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data(), num_sampler, sampler, rootSignatureFlags);

    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error))) {
        if (error) {
            OutputDebugStringA((char*)error->GetBufferPointer());
        }
        throw std::runtime_error("D3DX12SerializeVersionedRootSignature failed");
    }

    //���[�g�V�O�j�`���̍쐬
    if (FAILED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&this->root_signature_)))) {
        throw std::runtime_error("CreateRootSignature failed");
    }
}