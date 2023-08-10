#pragma once
#include "Scene.h"
#include <memory>

class Model;
class Camera;
class CameraController;

class Scene3dModel final : public Scene
{
public:
    Scene3dModel() {};
    ~Scene3dModel() {};

    //����������
    void init(ID3D12Device* device)override;
    //�I������
    void finalize()override;
    //�X�V����
    void update()override;
    //�`�揈��
    void render(RenderContext* rc)override;

    void updateImguiMenu() override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<CameraController> camera_controller_;
};
