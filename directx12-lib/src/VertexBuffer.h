#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// ���_�o�b�t�@�����p&copy����N���X
/// </summary>
class VertexBuffer
{
    friend class VertexBufferFactory;
public:
    /// <summary>
    /// ���_�o�b�t�@�������Ɏg�p����ݒ�
    /// </summary>
    struct VertexBufferConf {
        int size;               //���_�o�b�t�@�̃T�C�Y
        int stride;             //1���_�̃T�C�Y
    };

private:
    VertexBuffer(const VertexBufferConf c) :
        conf_(c),
        vertex_buffer_(),
        vertex_buffer_view_()
    {};

public:
    ~VertexBuffer() {};

private:
    //����������
    void init(ID3D12Device* device);

public:
    //���\�[�X�ɃR�s�[
    void copy(void* srcVertices);

public:
    //���_�o�b�t�@�r���[�̎擾
    const D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView() const { return this->vertex_buffer_view_; }

private:
    VertexBufferConf conf_;
    ComPtr<ID3D12Resource>      vertex_buffer_;	        //���_�o�b�t�@�B
    D3D12_VERTEX_BUFFER_VIEW    vertex_buffer_view_;	//���_�o�b�t�@�r���[�B
};
