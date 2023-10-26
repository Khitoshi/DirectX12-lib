#pragma once

#include "IndexBuffer.h"

#include <memory>

/// <summary>
/// インデックスバッファのファクトリクラス
/// </summary>
class IndexBufferFactory
{
public:
	static std::shared_ptr<IndexBuffer> create(const IndexBuffer::IndexBufferConf& conf, ID3D12Device* device) {
		std::shared_ptr<IndexBuffer> index_buffer(new IndexBuffer(conf));
		index_buffer->init(device);
		return index_buffer;
	}
};
