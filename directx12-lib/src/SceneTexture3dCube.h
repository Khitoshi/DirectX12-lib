#pragma once

#include "Scene.h"
#include <DirectXMath.h>
class Camera;
class CameraController;
class TextureCubeModel;

class SceneTexture3dCube final : public Scene
{
public:
    SceneTexture3dCube() :
        cube_model_(),
        camera_(),
        is_change_camera_(false),
        camera_angle_(DirectX::XMFLOAT3(0, 0, 0)),
        camera_controller_()
    {};
    ~SceneTexture3dCube() {};

    //����������
    void init(ID3D12Device* device)override;
    //�I������
    void finalize()override;
    //�X�V����
    void update()override;
    //�`�揈��
    void render(RenderContext* rc)override;
    //imgui��menu��\��
    void updateImguiMenu()override;
private:
    std::shared_ptr<TextureCubeModel> cube_model_;
    std::shared_ptr<Camera> camera_;
    bool is_change_camera_;
    DirectX::XMFLOAT3 camera_angle_;
    std::shared_ptr<CameraController> camera_controller_;
};
