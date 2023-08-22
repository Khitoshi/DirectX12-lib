#pragma once

#include "DescriptorHeap.h"
#include <memory>

/// <summary>
/// ディスクリプタヒープ生成クラス
/// </summary>
class DescriptorHeapFactory
{
public:
    /// <summary>
    /// 生成
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="type">ディスクリプタヒープタイプ</param>
    /// <param name="num_descriptors">ディスクリプタの数</param>
    /// <returns>
    /// 生成&初期化されたディスクリプタヒープ
    /// </returns>
    static std::shared_ptr<DescriptorHeap> create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_descriptors)
    {
        std::shared_ptr<DescriptorHeap> heap(new DescriptorHeap());
        heap->init(device, type, num_descriptors);

        return heap;
    }
};
