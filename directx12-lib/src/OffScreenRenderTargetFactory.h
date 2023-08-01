#pragma once
#include "OffScreenRenderTarget.h"
#include <memory>

/// <summary>
/// オフスクリーンレンダーターゲットのファクトリークラス
/// </summary>
class OffScreenRenderTargetFactory
{
public:
    /// <summary>
    /// オフスクリーンレンダーターゲットを生成する
    /// </summary>
    /// <param name="conf">生成に関する設定</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化したオフスクリーンレンダーターゲットのインスタンス
    /// </returns>
    static std::shared_ptr<OffScreenRenderTarget> create(const OffScreenRenderTarget::OffScreenRenderTargetConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<OffScreenRenderTarget> off_screen_render_target(new OffScreenRenderTarget(conf));
        off_screen_render_target->init(device);
        return off_screen_render_target;
    }
};
