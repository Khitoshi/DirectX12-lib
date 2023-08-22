#pragma once
#include "Scene.h"
#include <memory>
#include <string>

class Model;
class Camera;
class CameraController;

// assimp‚É‘Î‰‚·‚éƒ‚ƒfƒ‹‚ğ“Ç‚İ‚ñ‚Å•\¦‚·‚éscene
class SceneModel final : public Scene
{
public:
    SceneModel() :
        model(nullptr),
        camera_(nullptr),
        camera_controller_(nullptr),
        is_change_model_(false)
    {};
    ~SceneModel() {};

    void init(ID3D12Device* device)override;
    void finalize()override;
    void update()override;
    void render(RenderContext* rc)override;
    void updateImguiMenu() override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<CameraController> camera_controller_;
    static std::string file_path_;

    bool is_change_model_;
};
