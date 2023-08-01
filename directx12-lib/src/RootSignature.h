#pragma once
#include "d3dx12.h"

#include <vector>
#include <algorithm>

using namespace Microsoft::WRL;

/// <summary>
/// ���[�g�V�O�j�`���̐����p�N���X
/// </summary>
class RootSignature
{
    friend class RootSignatureFactory;
public:
    /// <summary>
    /// ���[�g�V�O�l�`���������Ɏg�p����ݒ�
    /// </summary>
    struct RootSignatureConf {
        D3D12_FILTER sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        D3D12_TEXTURE_ADDRESS_MODE texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        D3D12_TEXTURE_ADDRESS_MODE texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        D3D12_TEXTURE_ADDRESS_MODE texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        int num_sampler = 0;
        UINT max_cbv_descriptor = 0;
        UINT max_srv_descriptor = 0;
        UINT max_uav_descriptor = 0;
        UINT offset_in_descriptors_from_table_start_cb = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        UINT offset_in_descriptors_from_table_start_srv = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        UINT offset_in_descriptors_from_table_start_uav = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        bool operator==(const RootSignatureConf& other) const {
            return
                sampler_filter == other.sampler_filter &&

                (int)texture_address_modeU == (int)other.texture_address_modeU &&
                (int)texture_address_modeV == (int)other.texture_address_modeV &&
                (int)texture_address_modeW == (int)other.texture_address_modeW &&

                num_sampler == other.num_sampler &&

                max_cbv_descriptor == other.max_cbv_descriptor &&
                max_srv_descriptor == other.max_srv_descriptor &&
                max_uav_descriptor == other.max_uav_descriptor &&

                offset_in_descriptors_from_table_start_cb == other.offset_in_descriptors_from_table_start_cb &&
                offset_in_descriptors_from_table_start_srv == other.offset_in_descriptors_from_table_start_srv &&
                offset_in_descriptors_from_table_start_uav == other.offset_in_descriptors_from_table_start_uav &&

                root_signature_flags == other.root_signature_flags;
        }
    };

private:
    RootSignature(const RootSignatureConf c) :conf_(c), root_signature_() {}

public:
    ~RootSignature() {}

private:
    //����������
    void init(ID3D12Device* device);
    //���[�g�V�O�j�`������
    void createRootSignature(
        ID3D12Device* device,
        D3D12_STATIC_SAMPLER_DESC* sampler,
        const UINT max_cbv_descriptor = 0,
        const UINT max_srv_descriptor = 0,
        const UINT max_uav_descritor = 0,
        const UINT num_sampler = 0,
        const UINT offset_in_descriptors_from_table_Start_cb = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        const UINT offset_in_descriptors_from_table_Start_srv = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        const UINT offset_in_descriptors_from_table_Start_uav = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        const D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );

public:
    //���[�g�V�O�l�`���̎擾
    ID3D12RootSignature* getRootSignature() const { return this->root_signature_.Get(); }

private:
    RootSignatureConf conf_;                    //�ݒ�
    ComPtr<ID3D12RootSignature> root_signature_;//���[�g�V�O�l�`��
};
