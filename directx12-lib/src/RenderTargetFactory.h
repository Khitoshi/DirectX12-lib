#pragma once
#include "RenderTarget.h"
#include <memory>

/// <summary>
/// レンダーターゲットを生成するクラス
/// </summary>
class RenderTargetFactory
{
public:
    /// <summary>
    /// レンダーターゲットを生成する
    /// </summary>
    /// <param name="conf">生成に必要な情報</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化したレンダーターゲット
    /// </returns>
    static std::shared_ptr<RenderTarget> create(const RenderTarget::RenderTargetConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<RenderTarget> renderTarget(new RenderTarget(conf));
        renderTarget->init(device);
        return renderTarget;
    }

};
