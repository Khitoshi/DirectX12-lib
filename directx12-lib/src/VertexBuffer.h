#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// ���_�o�b�t�@�������Ɏg�p����ݒ�
/// </summary>
struct VertexBufferConf {
    ID3D12Device* device;
    int size;
    int stride;
};

/// <summary>
/// ���_�o�b�t�@�����p&copy����N���X
/// </summary>
class VertexBuffer
{
public:
    VertexBuffer() :vertexBuffer(), vertexBufferView() {};
    ~VertexBuffer() {};

    //����������
    void init(VertexBufferConf conf);
    //���\�[�X�ɃR�s�[
    void copy(void* srcVertices);

public:
    //���_�o�b�t�@�r���[�̎擾
    D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const { return vertexBufferView; }

private:
    ComPtr<ID3D12Resource>      vertexBuffer;		        //���_�o�b�t�@�B
    D3D12_VERTEX_BUFFER_VIEW    vertexBufferView;	        //���_�o�b�t�@�r���[�B
};
