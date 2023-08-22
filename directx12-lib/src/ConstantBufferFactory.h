#pragma once

#include "ConstantBuffer.h"
#include <memory>

/// <summary>
/// 定数バッファのファクトリークラス
/// </summary>
class ConstantBufferFactory
{
public:
    /// <summary>
    /// 生成関数
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化された定数バッファ
    /// </returns>
    static std::shared_ptr<ConstantBuffer> create(ID3D12Device* device, const ConstantBuffer::ConstantBufferConf& conf)
    {
        std::shared_ptr<ConstantBuffer> cb(new ConstantBuffer(conf));
        cb->init(device);
        return cb;
    }
};
