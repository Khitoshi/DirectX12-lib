#pragma once
#include "Sprite.h"
#include "Scene.h"
#include <memory>

class SceneSprite : public Scene
{
public:
    SceneSprite(){};
    ~SceneSprite(){};

    //初期化処理
    void init(SceneConf conf);
    //終了処理
    void finalize();
    //更新処理
    void update();
    //描画処理
    void render(SceneConf conf);
    //ImGuiメニュー
    void updateImguiMenu();
private:
    std::shared_ptr<Sprite> sprite;
};
