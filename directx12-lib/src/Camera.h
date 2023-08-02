#pragma once
#include <DirectXMath.h>

/// <summary>
/// �J����
/// </summary>
class Camera
{
public:
    Camera() :
        target_position_length_(1.0f),
        eye_(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f)),
        up_(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)),
        target_(DirectX::XMFLOAT3(0.0f, 0.0f, 0.01f)),
        view_matrix_(DirectX::XMMATRIX()),
        projection_matrix_(DirectX::XMMATRIX()),
        forward_(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)),
        right_(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)),
        near_(0.1f),
        far_(100.0f),
        view_angle_(DirectX::XMConvertToRadians(60.0f)),
        aspect_(0.0f)
    {};

    //����������
    void init(const float window_width, const float window_height);
    //�X�V����
    void update(const float window_width, const float window_height);

public:// �ݒ�n
    void setEye(const DirectX::XMFLOAT3 e) { this->eye_ = e; }
    void setTarget(const DirectX::XMFLOAT3 t) { this->target_ = t; }

    void setNear(const float n) { this->near_ = n; }
    void setFar(const float f) { this->far_ = f; }

public:// �擾�n
    DirectX::XMFLOAT3 getEye() const { return this->eye_; }
    DirectX::XMFLOAT3 getTarget() const { return this->target_; }
    DirectX::XMMATRIX getViewMatrix() const { return this->view_matrix_; }
    DirectX::XMMATRIX getProjectionMatrix() const { return this->projection_matrix_; }
    float getNear() const { return this->near_; }
    float getFar() const { return this->far_; }


private:
    float target_position_length_;          // �����_�Ƃ̋���

    DirectX::XMFLOAT3 eye_;                 // ���_
    DirectX::XMFLOAT3 up_;                  // �����
    DirectX::XMFLOAT3 target_;              // �����_

    DirectX::XMMATRIX view_matrix_;       // �r���[�s��
    DirectX::XMMATRIX projection_matrix_; // �����ϊ��s��

    DirectX::XMFLOAT3 forward_;             // �O����
    DirectX::XMFLOAT3 right_;               // �E����

    float near_;                            // �j�A�N���b�v
    float far_;                             // �t�@�[�N���b�v

    float view_angle_;                      // ����p
    float aspect_;                          // �A�X�y�N�g��
};
