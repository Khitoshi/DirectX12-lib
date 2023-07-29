#pragma once

#include "Scene.h"

/// <summary>
/// 何の処理もしないシーン
/// </summary>
class SceneDefault final : public Scene
{
public:
    SceneDefault() {};
    ~SceneDefault() {};

    //初期化処理
    void init(ID3D12Device* device)override {};
    //終了処理
    void finalize()override {};
    //更新処理
    void update()override {};
    //描画処理
    void render(RenderContext* rc)override {};
};
