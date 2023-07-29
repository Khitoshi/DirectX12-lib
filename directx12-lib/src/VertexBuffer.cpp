#include "VertexBuffer.h"
#include <stdexcept>

/// <summary>
/// ���_�o�b�t�@�̏�����
/// </summary>
/// <param name="conf">���_�o�b�t�@�����p�̐ݒ�</param>
void VertexBuffer::init(ID3D12Device* device)
{
    //�q�[�v�̐ݒ�
    D3D12_HEAP_PROPERTIES heap_prop = {};
    heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_prop.CreationNodeMask = 1;
    heap_prop.VisibleNodeMask = 1;
    heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = this->conf_.size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    if (FAILED(
        device->CreateCommittedResource(
            &heap_prop,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&this->vertex_buffer_)
        )
    )) {
        throw std::runtime_error("failed to create vertex buffer");
    }
    if (FAILED(this->vertex_buffer_->SetName(L"VertexBuffer"))) {
        throw std::runtime_error("failed to set name to vertex buffer");
    }

    //���_�o�b�t�@�r���[�̐ݒ�
    this->vertex_buffer_view_.BufferLocation = this->vertex_buffer_->GetGPUVirtualAddress();
    this->vertex_buffer_view_.SizeInBytes = this->conf_.size;
    this->vertex_buffer_view_.StrideInBytes = this->conf_.stride;
}

/// <summary>
/// ���\�[�X�ɒ��_�f�[�^���R�s�[����B
/// </summary>
/// <param name="srcVertices">���_�f�[�^�z��</param>
void VertexBuffer::copy(void* src_vertices)
{
    uint8_t* pData = nullptr;
    //�}�b�s���O�J�n
    if (FAILED(this->vertex_buffer_->Map(0, nullptr, (void**)&pData))) {
        throw std::runtime_error("failed to map vertex buffer");
    }
    //���_�f�[�^���}�b�s���O��ɓo�^
    memcpy(pData, src_vertices, this->vertex_buffer_view_.SizeInBytes);
    //�}�b�s���O�o�^
    this->vertex_buffer_->Unmap(0, nullptr);
}