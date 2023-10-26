#pragma once
#include "DepthStencil.h"
#include <memory>

class DepthStencilFactory
{
public:
	static std::shared_ptr<DepthStencil> create(const DepthStencil::DepthStencilConf& conf, ID3D12Device* device) {
		std::shared_ptr<DepthStencil> ds(new DepthStencil(conf));
		ds->init(device);
		return ds;
	}
};
