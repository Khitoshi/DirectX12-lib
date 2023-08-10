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

    //‰Šú‰»ˆ—
    void init(ID3D12Device* device)override;
    //I—¹ˆ—
    void finalize()override;
    //XVˆ—
    void update()override;
    //•`‰æˆ—
    void render(RenderContext* rc)override;

    void updateImguiMenu() override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<CameraController> camera_controller_;
};
