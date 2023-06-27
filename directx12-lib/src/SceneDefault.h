#pragma once

#include "Scene.h"

/// <summary>
/// 何の処理もしないシーン
/// </summary>
class SceneDefault: public Scene
{
public:
    SceneDefault() {};
    ~SceneDefault() {};

    //初期化処理
    void init(SceneConf conf) {};
    //終了処理
    void finalize() {};
    //更新処理
    void update() {};
    //描画処理
    void render(SceneConf conf) {};
};
