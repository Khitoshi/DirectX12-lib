#include "Texture.h"
#include "DescriptorHeap.h"
#include <stdexcept>

/// <summary>
/// �e�N�X�`���ǂݍ���
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
/// <param name="texture_file_path">�e�X�N�`���̃t�@�C���p�X</param>
void Texture::Load(ID3D12Device* device, const char* texture_file_path)
{
    //�t�@�C���p�X�����C�h�����ɕϊ�
    wchar_t w_file_Path[256];
    //MultiByteToWideChar(CP_ACP, 0, texture_file_path, -1, w_file_Path, _countof(w_file_Path));
    MultiByteToWideChar(CP_UTF8, 0, texture_file_path, -1, w_file_Path, _countof(w_file_Path));

    //DirectXTex���g���ăe�N�X�`����ǂݍ���
    DirectX::ScratchImage scratch_img = {};
    if (FAILED(LoadFromWICFile(w_file_Path, DirectX::WIC_FLAGS_NONE, &this->meta_data_, scratch_img))) {
        throw std::runtime_error("failed to load texture");
    }

    //���f�[�^���o
    auto img = scratch_img.GetImage(0, 0, 0);

    //WriteToSubresource�œ]������p�̃q�[�v�ݒ�
    D3D12_HEAP_PROPERTIES tex_heap_prop = {};
    tex_heap_prop.Type = D3D12_HEAP_TYPE_CUSTOM;
    tex_heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    tex_heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
    tex_heap_prop.CreationNodeMask = 0;
    tex_heap_prop.VisibleNodeMask = 0;

    //�e�N�X�`���̐ݒ�
    D3D12_RESOURCE_DESC tex_desc = {};
    tex_desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(this->meta_data_.dimension);
    tex_desc.Alignment = 0;
    tex_desc.Width = this->meta_data_.width;
    tex_desc.Height = static_cast<UINT>(this->meta_data_.height);
    tex_desc.DepthOrArraySize = static_cast<UINT16>(this->meta_data_.arraySize);
    tex_desc.MipLevels = static_cast<UINT16>(this->meta_data_.mipLevels);
    tex_desc.Format = this->meta_data_.format;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    tex_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    //�e�N�X�`���̍쐬
    if (FAILED(device->CreateCommittedResource(
        &tex_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &tex_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&this->resource_)))) {
        throw std::runtime_error("failed to create texture");
    }

    //�e�N�X�`���̓]��
    if (FAILED(this->resource_->WriteToSubresource(
        0,
        nullptr,
        img->pixels,
        static_cast<UINT>(img->rowPitch),
        static_cast<UINT>(img->slicePitch)))) {
        throw std::runtime_error("failed to write texture");
    }

}

/// <summary>
/// �V�F�[�_�[���\�[�X�r���[�̍쐬
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
/// <param name="descriptor_heap">���\�[�X���i�[�������f�B�X�N���v�^�q�[�v</param>
/// <param name="slot">���\�[�X���i�[�������|�C���^�̈ʒu</param>
void Texture::CreateShaderResourceView(ID3D12Device* device, DescriptorHeap* descriptor_heap, const UINT slot)
{
    //�ʏ�e�N�X�`���r���[�쐬
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = this->meta_data_.format;//RGBA(0.0f�`1.0f�ɐ��K��)
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
    srv_desc.Texture2D.MipLevels = 1;
    auto handle = descriptor_heap->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * slot;
    device->CreateShaderResourceView(this->resource_.Get(), &srv_desc, handle);
}