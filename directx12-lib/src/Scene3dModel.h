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

    //初期化処理
    void init(ID3D12Device* device)override;
    //終了処理
    void finalize()override;
    //更新処理
    void update()override;
    //描画処理
    void render(RenderContext* rc)override;

    void updateImguiMenu() override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<CameraController> camera_controller_;
};
