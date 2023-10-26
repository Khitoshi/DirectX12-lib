#pragma once

#include "VertexBuffer.h"
#include <memory>

/// <summary>
/// ���_�o�b�t�@�̃t�@�N�g���N���X
/// </summary>
class VertexBufferFactory
{
public:
	static std::shared_ptr<VertexBuffer> create(const VertexBuffer::VertexBufferConf& conf, ID3D12Device* device)
	{
		std::shared_ptr<VertexBuffer> vertexBuffer(new VertexBuffer(conf));
		vertexBuffer->init(device);
		return vertexBuffer;
	}
};
