#pragma once
#include "d3dx12.h"
#include <dxgi1_4.h>
using namespace Microsoft::WRL;

/// <summary>
/// �f�o�C�X�R���e�L�X�g
/// </summary>
class DeviceContext
{
    friend class DeviceContextFactory;
private:
    DeviceContext() :device_() {};

public:
    ~DeviceContext() {};

private:
    //������
    void init(IDXGIFactory4* dxgiFactory);

public://�擾�n
    //�f�o�C�X���擾
    ID3D12Device5* getDevice() { return device_.Get(); }

private:
    ComPtr<ID3D12Device5>device_;   //GPU�f�o�C�X
};
