#pragma once
#include <memory>
#include "SRV.h"

class SRVFactory
{
public:
	static std::shared_ptr<SRV> create(ID3D12Device* device, const D3D12_HEAP_PROPERTIES& heap_prop, const D3D12_RESOURCE_DESC& desc)
	{
		std::shared_ptr<SRV> srv(new SRV());
		srv->init(device, heap_prop, desc);

		return srv;
	}
};
