#pragma once
#include <memory>
#include "DeviceContext.h"

/// <summary>
/// デバイスコンテキストファクトリクラス
/// </summary>
class DeviceContextFactory
{
public:
	static std::shared_ptr<DeviceContext> create(IDXGIFactory4* dxgiFactory) {
		std::shared_ptr<DeviceContext> deviceContext(new DeviceContext());
		deviceContext->init(dxgiFactory);
		return deviceContext;
	}
};
