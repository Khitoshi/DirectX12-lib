#pragma once
#pragma once

#include "Scene.h"
#include <memory>

class CubeModel;
class Camera;

/// <summary>
/// 何の処理もしないシーン
/// </summary>
class Scene3dCube final : public Scene
{
public:
    Scene3dCube() :
        cube_model_(),
        camera_(),
        is_change_camera_(false)
    {};
    ~Scene3dCube() {};

    //初期化処理
    void init(ID3D12Device* device)override;
    //終了処理
    void finalize()override;
    //更新処理
    void update()override;
    //描画処理
    void render(RenderContext* rc)override;
    //imguiのmenuを表示
    void updateImguiMenu()override;
private:
    std::shared_ptr<CubeModel> cube_model_;
    std::shared_ptr<Camera> camera_;
    bool is_change_camera_;
};
