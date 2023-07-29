#pragma once
#include "d3dx12.h"
#include <dxgi1_4.h>
using namespace Microsoft::WRL;

/// <summary>
/// デバイスコンテキスト
/// </summary>
class DeviceContext
{
    friend class DeviceContextFactory;
private:
    DeviceContext() :device_() {};

public:
    ~DeviceContext() {};

private:
    //初期化
    void init(IDXGIFactory4* dxgiFactory);

public://取得系
    //デバイスを取得
    ID3D12Device5* getDevice() { return device_.Get(); }

private:
    ComPtr<ID3D12Device5>device_;   //GPUデバイス
};
