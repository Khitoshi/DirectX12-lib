#pragma once
#include "DepthStencil.h"
#include <memory>

class DepthStencilFactory
{
public:

    /// <summary>
    /// 深度ステンシルを作成する
    /// </summary>
    /// <param name="conf">生成に必要な設定</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 深度ステンシル
    /// </returns>
    static std::shared_ptr<DepthStencil> create(const DepthStencil::DepthStencilConf& conf, ID3D12Device* device) {
        std::shared_ptr<DepthStencil> ds(new DepthStencil(conf));
        ds->init(device);
        return ds;
    }
};
