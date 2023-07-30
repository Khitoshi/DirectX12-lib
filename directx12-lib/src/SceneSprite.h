#pragma once

#include "Scene.h"
#include "Sprite.h"
#include <memory>

class Camera;

/// <summary>
/// スプライト描画用シーン
/// </summary>
class SceneSprite final : public Scene
{
public:
    SceneSprite() :
        sprite_(),
        vertex_(),
        is_change_vertex_(false),
        device_(nullptr),
        is_change_texture_(false)
    {};
    ~SceneSprite() {};

    //初期化処理
    void init(ID3D12Device* device)override;
    //終了処理
    void finalize()override;
    //更新処理
    void update()override;
    //描画処理
    void render(RenderContext* rc)override;
    //ImGuiメニュー
    void updateImguiMenu()override;

private:
    std::shared_ptr<Sprite> sprite_;    //スプライト
    Sprite::Vertex vertex_[4];          //頂点座標
    bool is_change_vertex_;             //頂点座標を変更したか

    ID3D12Device* device_;              //デバイス

    static std::string file_path_;      //ファイルパス
    bool is_change_texture_;            //テクスチャを変更したか
};
