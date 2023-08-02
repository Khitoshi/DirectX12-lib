#include "Camera.h"

/// <summary>
/// ����������
/// </summary>
/// <param name="window_width"></param>
/// <param name="window_height"></param>
void Camera::init(const float window_width, const float window_height)
{
    this->aspect_ = window_width / window_height;

    //���e�s��̍쐬
    this->projection_matrix_ = DirectX::XMMatrixPerspectiveFovLH(
        this->view_angle_,
        this->aspect_,
        this->near_,
        this->far_);

    //�r���[�s��̍쐬
    this->view_matrix_ = DirectX::XMMatrixLookAtLH(
        DirectX::XMLoadFloat3(&this->eye_),
        DirectX::XMLoadFloat3(&this->target_),
        DirectX::XMLoadFloat3(&this->up_));



}


/// <summary>
/// �X�V����
/// </summary>
/// <param name="window_width"></param>
/// <param name="window_height"></param>
void Camera::update(const float window_width, const float window_height)
{
    this->aspect_ = window_width / window_height;

    //���e�s��̍쐬
    this->projection_matrix_ = DirectX::XMMatrixPerspectiveFovLH(
        this->view_angle_,
        this->aspect_,
        this->near_,
        this->far_);

    //�r���[�s��̍쐬
    this->view_matrix_ = DirectX::XMMatrixLookAtLH(
        DirectX::XMLoadFloat3(&this->eye_),
        DirectX::XMLoadFloat3(&this->target_),
        DirectX::XMLoadFloat3(&this->up_));
}
