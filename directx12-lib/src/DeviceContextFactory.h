#pragma once
#include <memory>
#include "DeviceContext.h"

/// <summary>
/// デバイスコンテキスト生成クラス
/// </summary>
class DeviceContextFactory
{
public:
    /// <summary>
    /// デバイスコンテキストを生成する
    /// </summary>
    /// <param name="dxgiFactory">dxgiFactory</param>
    /// <returns>
    /// 生成&初期化されたデバイスコンテキスト
    /// </returns>
    static std::shared_ptr<DeviceContext> create(IDXGIFactory4* dxgiFactory) {
        std::shared_ptr<DeviceContext> deviceContext(new DeviceContext());
        deviceContext->init(dxgiFactory);
        return deviceContext;
    }
};
