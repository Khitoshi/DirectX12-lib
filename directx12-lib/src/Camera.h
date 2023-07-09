#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera(int width,int height):
		eye(0,0,-5),
		target(0,0,0),
		up(0,1,0),
		width(width),
		height(height),
		worldMatrix(DirectX::XMMatrixRotationY(DirectX::XM_PIDIV2))
	{};
	~Camera() {};
	void init();
	void update();
	DirectX::XMMATRIX getViewMatrix()const
	{
		return DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	}
	DirectX::XMMATRIX getProjectionMatrix()const
	{
		//âÊäp90Åã
		return DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XM_PIDIV2,
			static_cast<float>(width) / static_cast<float>(height),//ÉAÉXî‰,
			1.0f, 10.0f);
	}
	DirectX::XMMATRIX getWorldMatrix()const
	{
		return this->worldMatrix;
	}
private:
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 target;
	DirectX::XMFLOAT3 up;
	int width;
	int height;
	DirectX::XMMATRIX worldMatrix;
};
