#pragma once

#include "VertexBuffer.h"
#include <memory>

/// <summary>
/// ���_�o�b�t�@�̃t�@�N�g���N���X
/// </summary>
class VertexBufferFactory
{
public:
    /// <summary>
    /// ���_�o�b�t�@�𐶐�����
    /// </summary>
    /// <param name="conf">�����Ɋւ���ݒ�</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&�������������_�o�b�t�@
    /// </returns>
    static std::shared_ptr<VertexBuffer> create(const VertexBuffer::VertexBufferConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<VertexBuffer> vertexBuffer(new VertexBuffer(conf));
        vertexBuffer->init(device);
        return vertexBuffer;
    }
};
