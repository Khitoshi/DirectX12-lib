#pragma once
#include <memory>
#include "DeviceContext.h"

/// <summary>
/// �f�o�C�X�R���e�L�X�g�����N���X
/// </summary>
class DeviceContextFactory
{
public:
    /// <summary>
    /// �f�o�C�X�R���e�L�X�g�𐶐�����
    /// </summary>
    /// <param name="dxgiFactory">dxgiFactory</param>
    /// <returns>
    /// ����&���������ꂽ�f�o�C�X�R���e�L�X�g
    /// </returns>
    static std::shared_ptr<DeviceContext> create(IDXGIFactory4* dxgiFactory) {
        std::shared_ptr<DeviceContext> deviceContext(new DeviceContext());
        deviceContext->init(dxgiFactory);
        return deviceContext;
    }
};
