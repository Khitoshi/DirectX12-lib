#pragma once
#include "Scene.h"
#include <memory>
#include <string>
class FBXModel;
class Camera;
class CameraController;

class SceneFBXModel final : public Scene
{
public:
    SceneFBXModel() :
        model(nullptr),
        camera_(nullptr),
        camera_controller_(nullptr),
        is_change_model_(false)
    {};
    ~SceneFBXModel() {};

    void init(ID3D12Device* device)override;
    void finalize()override;
    void update()override;
    void render(RenderContext* rc)override;
    void updateImguiMenu() override;

private:
    std::shared_ptr<FBXModel> model;
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<CameraController> camera_controller_;
    static std::string file_path_;

    bool is_change_model_;
};
