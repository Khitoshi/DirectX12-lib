#pragma once
#include <DirectXMath.h>

/// <summary>
/// カメラ
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

    //初期化処理
    void init(const float window_width, const float window_height);
    //更新処理
    void update(const float window_width, const float window_height);

public:// 設定系
    void setEye(const DirectX::XMFLOAT3 e) { this->eye_ = e; }
    void setTarget(const DirectX::XMFLOAT3 t) { this->target_ = t; }

    void setNear(const float n) { this->near_ = n; }
    void setFar(const float f) { this->far_ = f; }

public:// 取得系
    DirectX::XMFLOAT3 getEye() const { return this->eye_; }
    DirectX::XMFLOAT3 getTarget() const { return this->target_; }
    DirectX::XMMATRIX getViewMatrix() const { return this->view_matrix_; }
    DirectX::XMMATRIX getProjectionMatrix() const { return this->projection_matrix_; }
    float getNear() const { return this->near_; }
    float getFar() const { return this->far_; }


private:
    float target_position_length_;          // 注視点との距離

    DirectX::XMFLOAT3 eye_;                 // 視点
    DirectX::XMFLOAT3 up_;                  // 上方向
    DirectX::XMFLOAT3 target_;              // 注視点

    DirectX::XMMATRIX view_matrix_;       // ビュー行列
    DirectX::XMMATRIX projection_matrix_; // 透視変換行列

    DirectX::XMFLOAT3 forward_;             // 前方向
    DirectX::XMFLOAT3 right_;               // 右方向

    float near_;                            // ニアクリップ
    float far_;                             // ファークリップ

    float view_angle_;                      // 視野角
    float aspect_;                          // アスペクト比
};
