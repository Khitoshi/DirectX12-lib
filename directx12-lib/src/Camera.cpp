#include "Camera.h"

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

void Camera::update(const float window_width, const float window_height)
{
	this->aspect_ = window_width / window_height;

	//投影行列の作成
	this->projection_matrix_ = DirectX::XMMatrixPerspectiveFovLH(
		this->view_angle_,
		this->aspect_,
		this->near_,
		this->far_);

}

void Camera::rotate(const int x, const int y)
{
	static int prev_x = 0;
	static int prev_y = 0;
	static bool is_first = true;
	if (is_first) {//(初回のみ)
		//マウス座標更新して早期リターン
		prev_x = x;
		prev_y = y;
		is_first = false;
		return;
	}

	//マウス座標更新
	const int delta_x = x - prev_x;
	const int delta_y = y - prev_y;

	static DirectX::XMFLOAT3 camera_angle = { DirectX::XMFLOAT3(0,0,0) };
	camera_angle.x += delta_y * (DirectX::XMConvertToRadians(60.0f) * 0.005f);
	camera_angle.y += delta_x * (DirectX::XMConvertToRadians(60.0f) * 0.005f);
	camera_angle.z += 0.0f;

	DirectX::XMMATRIX rotation_transform = DirectX::XMMatrixRotationRollPitchYaw(camera_angle.x, camera_angle.y, camera_angle.z);

	DirectX::XMStoreFloat3(&this->forward_, DirectX::XMVector3Normalize(rotation_transform.r[2]));
	DirectX::XMStoreFloat3(&this->right_, DirectX::XMVector3Normalize(rotation_transform.r[0]));

	this->eye_.x = this->target_.x - this->forward_.x * this->target_position_length_;
	this->eye_.y = this->target_.y - this->forward_.y * this->target_position_length_;
	this->eye_.z = this->target_.z - this->forward_.z * this->target_position_length_;

	this->view_matrix_ = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&this->eye_),
		DirectX::XMLoadFloat3(&this->target_),
		DirectX::XMLoadFloat3(&this->up_));

	prev_x = x;
	prev_y = y;
}
