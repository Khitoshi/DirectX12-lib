#pragma once
#include <memory>
#include "CompositeRenderTarget.h"

class CompositeRenderTargetFactory
{
public:
    /// <summary>
    /// コンポジットレンダーターゲットを生成する
    /// </summary>
    /// <param name="crt_conf">生成に必要な設定</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化したコンポジットレンダーターゲット
    /// </returns>
    static std::shared_ptr<CompositeRenderTarget> create(const CompositeRenderTarget::CompositeRenderTargetConf& crt_conf, ID3D12Device* device)
    {
        std::shared_ptr<CompositeRenderTarget> compositeRenderTarget(new CompositeRenderTarget(crt_conf));
        compositeRenderTarget->init(device);
        return compositeRenderTarget;
    }
};
