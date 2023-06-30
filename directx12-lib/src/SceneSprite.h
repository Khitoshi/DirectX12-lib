#pragma once
#include "Sprite.h"
#include "Scene.h"
#include <memory>

/// <summary>
/// スプライト描画用シーン
/// </summary>
class SceneSprite : public Scene
{
public:
    SceneSprite() :
        sprite(),
        vertex(),
        isChangeVertex(false)
    {};
    ~SceneSprite() {};

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
    enum ImageFeature
    {
        FEATURE_NONE = 0,
        FEATURE_ROTATE = 1 << 0, // 0001
        //FEATURE_SCALE = 1 << 1,  // 0010
        //FEATURE_TRANSLATE = 1 << 2 // 0100
        // 更に追加機能があればここに定義します...
    };
    unsigned int imageFeatures = FEATURE_NONE;

    std::shared_ptr<Sprite> sprite;         //スプライト
    Sprite::Vertex vertex[4];               //頂点座標
    bool isChangeVertex;                    //頂点座標を変更したか
};
