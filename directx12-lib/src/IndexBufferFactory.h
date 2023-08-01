#pragma once

#include "IndexBuffer.h"

#include <memory>

/// <summary>
/// �C���f�b�N�X�o�b�t�@�̐������s���N���X
/// </summary>
class IndexBufferFactory
{
public:
    /// <summary>
    /// �C���f�b�N�X�o�b�t�@�𐶐�����
    /// </summary>
    /// <param name="conf">�����ɕK�v�Ȑݒ�</param>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <returns>
    /// ����&�����������C���f�b�N�X�o�b�t�@
    /// </returns>
    static std::shared_ptr<IndexBuffer> create(const IndexBuffer::IndexBufferConf& conf, ID3D12Device* device) {
        std::shared_ptr<IndexBuffer> index_buffer(new IndexBuffer(conf));
        index_buffer->init(device);
        return index_buffer;
    }
};
