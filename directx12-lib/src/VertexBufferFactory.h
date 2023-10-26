#pragma once

#include "VertexBuffer.h"
#include <memory>

/// <summary>
/// 頂点バッファのファクトリクラス
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
