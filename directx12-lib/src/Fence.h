#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// GPU�Ɠ�������I�u�W�F�N�g�����N���X
/// </summary>
class Fence
{
public:
    Fence() :
        fence(),
        value(0),
        event()
    {};
    ~Fence() {};

    //����������
    void init(ID3D12Device* device);

private:
    //�t�F���X�̐���
    ComPtr<ID3D12Fence> createFence(ID3D12Device* device);

    //�t�F���X�̒l�̐���
    UINT64 createValue();

    //�t�F���X�̃C�x���g�̐���
    HANDLE createEvent();
public:
    ID3D12Fence* getFence() const { return fence.Get(); } //�t�F���X�̎擾
    UINT64 getValue() const { return value; }            //�t�F���X�̒l�̎擾
    HANDLE getEvent() const { return event; }            //�t�F���X�̃C�x���g�̎擾

    void incrementValue() { value++; }                   //�t�F���X�̒l�̃C���N�������g
private:
    ComPtr<ID3D12Fence> fence;  //�t�F���X
    UINT64 value;               //�t�F���X�̒l
    HANDLE event;               //�t�F���X�̃C�x���g
};
