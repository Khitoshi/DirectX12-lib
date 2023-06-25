#include "Fence.h"
#include <stdexcept>

/// <summary>
/// GPU�Ɠ����I�u�W�F�N�g�̏�����
/// </summary>
/// <param name="device">�f�o�C�X�C���^�[�t�F�[�X</param>
void Fence::init(ID3D12Device* device)
{
    fence = createFence(device);
    value = createValue();
    event = createEvent();
}

/// <summary>
/// Fence�̐���
/// </summary>
/// <param name="device"></param>
/// <returns>
/// GPU�Ɠ����I�u�W�F�N�g
/// </returns>
ComPtr<ID3D12Fence> Fence::createFence(ID3D12Device* device)
{
    ComPtr<ID3D12Fence> f;
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&f)))) {
        throw std::runtime_error("failed to create fence");
    }

    return f;
}

/// <summary>
/// fence�̏����l�̐���(�ݒ�)
/// </summary>
/// <returns>
/// �ݒ肵�������l
/// </returns>
UINT64 Fence::createValue()
{
    UINT value = 1;
    return value;
}

/// <summary>
/// �C�x���g�̐���
/// </summary>
/// <returns>
/// ���������C�x���g
/// </returns>
HANDLE Fence::createEvent()
{
    HANDLE e = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (e == nullptr) {
        throw std::runtime_error("failed to create event");
    }

    return e;
}