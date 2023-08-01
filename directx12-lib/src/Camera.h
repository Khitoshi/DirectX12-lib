#pragma once
#include <DirectXMath.h>

/// <summary>
/// �J����
/// </summary>
class Camera
{
public:
    Camera(int width, int height) :
        eye_(0, 0, -5),
        target_(0, 0, 0),
        up_(0, 1, 0),
        width_(width),
        height_(height),
        world_matrix_(DirectX::XMMatrixRotationY(DirectX::XM_PIDIV2))
    {};
    ~Camera() {};

    //������
    void init();
    //�X�V
    void update();

public://�擾�n
    DirectX::XMMATRIX getViewMatrix()const
    {
        return DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye_), XMLoadFloat3(&target_), XMLoadFloat3(&up_));
    }
    DirectX::XMMATRIX getProjectionMatrix()const
    {
        //��p90��
        return DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XM_PIDIV2,
            static_cast<float>(width_) / static_cast<float>(height_),//�A�X��,
            1.0f, 10.0f);
    }
    DirectX::XMMATRIX getWorldMatrix()const
    {
        return this->world_matrix_;
    }
private:
    DirectX::XMFLOAT3 eye_;             //�J�����̈ʒu
    DirectX::XMFLOAT3 target_;          //�J�����̒����_
    DirectX::XMFLOAT3 up_;              //�J�����̏����

    int width_;                         //��ʂ̉���
    int height_;                        //��ʂ̏c��

    DirectX::XMMATRIX world_matrix_;    //���[���h�s��
};
