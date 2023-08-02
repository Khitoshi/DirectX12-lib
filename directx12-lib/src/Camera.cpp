#include "Camera.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="window_width"></param>
/// <param name="window_height"></param>
void Camera::init(const float window_width, const float window_height)
{
    this->aspect_ = window_width / window_height;

    //投影行列の作成
    this->projection_matrix_ = DirectX::XMMatrixPerspectiveFovLH(
        this->view_angle_,
        this->aspect_,
        this->near_,
        this->far_);

    //ビュー行列の作成
    this->view_matrix_ = DirectX::XMMatrixLookAtLH(
        DirectX::XMLoadFloat3(&this->eye_),
        DirectX::XMLoadFloat3(&this->target_),
        DirectX::XMLoadFloat3(&this->up_));



}


/// <summary>
/// 更新処理
/// </summary>
/// <param name="window_width"></param>
/// <param name="window_height"></param>
void Camera::update(const float window_width, const float window_height)
{
    this->aspect_ = window_width / window_height;

    //投影行列の作成
    this->projection_matrix_ = DirectX::XMMatrixPerspectiveFovLH(
        this->view_angle_,
        this->aspect_,
        this->near_,
        this->far_);

    //ビュー行列の作成
    this->view_matrix_ = DirectX::XMMatrixLookAtLH(
        DirectX::XMLoadFloat3(&this->eye_),
        DirectX::XMLoadFloat3(&this->target_),
        DirectX::XMLoadFloat3(&this->up_));
}
