#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// GPU�Ɠ�������I�u�W�F�N�g�����N���X
/// </summary>
class Fence
{
    friend class FenceFactory;
private:
    Fence() :
        fence_(),
        value_(0),
        event_()
    {};

public:
    ~Fence() {};

private:
    //����������
    void init(ID3D12Device* device);
    //�t�F���X�̐���
    void createFence(ID3D12Device* device);
    //�t�F���X�̒l�̐���
    void createValue();
    //�t�F���X�̃C�x���g�̐���
    void createEvent();

public://�擾�n
    //�t�F���X�̎擾
    ID3D12Fence* getFence() const { return this->fence_.Get(); }
    //�t�F���X�̒l�̎擾
    UINT64 getValue() const { return this->value_; }
    //�t�F���X�̃C�x���g�̎擾
    HANDLE getEvent() const { return this->event_; }

    //�t�F���X�̒l�̃C���N�������g
    void incrementValue() { this->value_++; }
private:
    ComPtr<ID3D12Fence> fence_;  //�t�F���X
    UINT64 value_;               //�t�F���X�̒l
    HANDLE event_;               //�t�F���X�̃C�x���g
};
