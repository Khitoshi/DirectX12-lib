#pragma once

#include "IndexBuffer.h"

#include <memory>

/// <summary>
/// インデックスバッファの生成を行うクラス
/// </summary>
class IndexBufferFactory
{
public:
    /// <summary>
    /// インデックスバッファを生成する
    /// </summary>
    /// <param name="conf">生成に必要な設定</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化したインデックスバッファ
    /// </returns>
    static std::shared_ptr<IndexBuffer> create(const IndexBuffer::IndexBufferConf& conf, ID3D12Device* device) {
        std::shared_ptr<IndexBuffer> index_buffer(new IndexBuffer(conf));
        index_buffer->init(device);
        return index_buffer;
    }
};
