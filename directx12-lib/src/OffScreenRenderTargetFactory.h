#pragma once
#include "OffScreenRenderTarget.h"
#include <memory>

class OffScreenRenderTargetFactory
{
public:
    static std::shared_ptr<OffScreenRenderTarget> create(const OffScreenRenderTarget::OffScreenRenderTargetConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<OffScreenRenderTarget> offScreenRenderTarget(new OffScreenRenderTarget(conf));
        offScreenRenderTarget->init(device);
        return offScreenRenderTarget;
    }
};
