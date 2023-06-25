#include "VertexBuffer.h"
#include <stdexcept>

/// <summary>
/// ���_�o�b�t�@�̏�����
/// </summary>
/// <param name="conf">���_�o�b�t�@�����p�̐ݒ�</param>
void VertexBuffer::init(VertexBufferConf conf)
{
    //�q�[�v�̐ݒ�
    D3D12_HEAP_PROPERTIES heapProp = {};
    heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProp.CreationNodeMask = 1;
    heapProp.VisibleNodeMask = 1;
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = conf.size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    if (FAILED(
        conf.device->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBuffer)
        )
    )) {
        throw std::runtime_error("failed to create vertex buffer");
    }
    vertexBuffer->SetName(L"VertexBuffer");

    //���_�o�b�t�@�r���[�̐ݒ�
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = conf.size;
    vertexBufferView.StrideInBytes = conf.stride;
}

/// <summary>
/// ���\�[�X�ɒ��_�f�[�^���R�s�[����B
/// </summary>
/// <param name="srcVertices">���_�f�[�^�z��</param>
void VertexBuffer::copy(void* srcVertices)
{
    uint8_t* pData = nullptr;
    //�}�b�s���O�J�n
    if (FAILED(vertexBuffer->Map(0, nullptr, (void**)&pData))) {
        throw std::runtime_error("failed to map vertex buffer");
    }
    //���_�f�[�^���}�b�s���O��ɓo�^
    memcpy(pData, srcVertices, vertexBufferView.SizeInBytes);
    //�}�b�s���O�o�^
    vertexBuffer->Unmap(0, nullptr);
}
