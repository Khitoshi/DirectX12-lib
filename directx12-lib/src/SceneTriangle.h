#pragma once
#include "Scene.h"
#include "Triangle.h"

/// <summary>
/// 三角形描画用シーン
/// </summary>
class SceneTriangle : public Scene
{
public:
    SceneTriangle() :
        triangle(),
        vertex(),
        isChangeVertex(false)
    {};
    ~SceneTriangle() {};

    //初期化処理
    void init(SceneConf conf);
    //終了処理
    void finalize();
    //更新処理
    void update();
    //描画処理
    void render(SceneConf conf);
    //imguiのmenuを表示
    void updateImguiMenu();
private:
    std::shared_ptr<Triangle> triangle;         //三角形
    Triangle::Vertex vertex[3];                 //頂点座標
    bool isChangeVertex;                        //頂点座標を変更したか
};
