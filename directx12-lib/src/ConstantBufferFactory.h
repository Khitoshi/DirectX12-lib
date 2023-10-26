#pragma once

#include "ConstantBuffer.h"
#include <memory>

/// <summary>
/// 定数バッファのファクトリークラス
/// </summary>
class ConstantBufferFactory
{
public:
	static std::shared_ptr<ConstantBuffer> create(ID3D12Device* device, const ConstantBuffer::ConstantBufferConf& conf)
	{
		std::shared_ptr<ConstantBuffer> cb(new ConstantBuffer(conf));
		cb->init(device);
		return cb;
	}
};
