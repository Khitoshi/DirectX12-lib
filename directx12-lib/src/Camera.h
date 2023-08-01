#pragma once
#include <DirectXMath.h>

/// <summary>
/// カメラ
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

    //初期化
    void init();
    //更新
    void update();

public://取得系
    DirectX::XMMATRIX getViewMatrix()const
    {
        return DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye_), XMLoadFloat3(&target_), XMLoadFloat3(&up_));
    }
    DirectX::XMMATRIX getProjectionMatrix()const
    {
        //画角90°
        return DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XM_PIDIV2,
            static_cast<float>(width_) / static_cast<float>(height_),//アス比,
            1.0f, 10.0f);
    }
    DirectX::XMMATRIX getWorldMatrix()const
    {
        return this->world_matrix_;
    }
private:
    DirectX::XMFLOAT3 eye_;             //カメラの位置
    DirectX::XMFLOAT3 target_;          //カメラの注視点
    DirectX::XMFLOAT3 up_;              //カメラの上方向

    int width_;                         //画面の横幅
    int height_;                        //画面の縦幅

    DirectX::XMMATRIX world_matrix_;    //ワールド行列
};
