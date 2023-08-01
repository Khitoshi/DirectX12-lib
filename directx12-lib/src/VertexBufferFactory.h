#pragma once

#include "VertexBuffer.h"
#include <memory>

/// <summary>
/// 頂点バッファのファクトリクラス
/// </summary>
class VertexBufferFactory
{
public:
    /// <summary>
    /// 頂点バッファを生成する
    /// </summary>
    /// <param name="conf">生成に関する設定</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化した頂点バッファ
    /// </returns>
    static std::shared_ptr<VertexBuffer> create(const VertexBuffer::VertexBufferConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<VertexBuffer> vertexBuffer(new VertexBuffer(conf));
        vertexBuffer->init(device);
        return vertexBuffer;
    }
};
