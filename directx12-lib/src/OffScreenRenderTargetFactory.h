#pragma once
#include "OffScreenRenderTarget.h"
#include <memory>

/// <summary>
/// オフスクリーンレンダーターゲットファクトリークラス
/// </summary>
class OffScreenRenderTargetFactory
{
public:
	static std::shared_ptr<OffScreenRenderTarget> create(const OffScreenRenderTarget::OffScreenRenderTargetConf& conf, ID3D12Device* device)
	{
		std::shared_ptr<OffScreenRenderTarget> off_screen_render_target(new OffScreenRenderTarget(conf));
		off_screen_render_target->init(device);
		return off_screen_render_target;
	}
};
