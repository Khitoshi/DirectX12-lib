#include "SceneSprite.h"
#include "Camera.h"
#include "imgui/imgui.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">GPUデバイス</param>
void SceneSprite::init(ID3D12Device* device)
{
    this->camera_ = std::make_shared<Camera>(1280, 720);
    this->camera_->init();

    //3角形のリソースを作成&初期化
    Sprite::SpriteConf sprite_conf = {};
    sprite_conf.file_path = "asset/img/test_image.png";
    sprite_conf.camera = this->camera_.get();
    this->sprite_ = std::make_shared<Sprite>(sprite_conf);
    this->sprite_->init(device);

    //頂点座標を設定
    this->vertex_[0] = this->sprite_->getVertices(0);
    this->vertex_[1] = this->sprite_->getVertices(1);
    this->vertex_[2] = this->sprite_->getVertices(2);
    this->vertex_[3] = this->sprite_->getVertices(3);
}

/// <summary>
/// 終了化処理
/// </summary>
void SceneSprite::finalize()
{
}

/// <summary>
/// 更新処理
/// </summary>
void SceneSprite::update()
{
    this->camera_->update();

    if (this->is_change_vertex_) {
        this->sprite_->setVertices(this->vertex_);
        this->is_change_vertex_ = false;
    }
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc"></param>
void SceneSprite::render(RenderContext* rc)
{
    this->sprite_->draw(rc);
}

/// <summary>
/// sceneで使用するimguiのメニュー更新処理
/// </summary>
void SceneSprite::updateImguiMenu()
{
    ImGui::Begin("Sprite");

    //頂点の位置
    for (int i = 0; i < 4; ++i) {
        float position[3] = { this->vertex_[i].position.x, this->vertex_[i].position.y, this->vertex_[i].position.z };
        std::string label = "Vertex pos" + std::to_string(i + 1);

        if (ImGui::SliderFloat3(label.c_str(), position, -1.0f, 1.0f)) {
            this->vertex_[i].position.x = position[0];
            this->vertex_[i].position.y = position[1];
            this->vertex_[i].position.z = position[2];
            this->is_change_vertex_ = true;
        }
    }

    //頂点の位置
    for (int i = 0; i < 4; ++i) {
        float uv[2] = { this->vertex_[i].uv.x, this->vertex_[i].uv.y, };
        std::string label = "Vertex uv" + std::to_string(i + 1);

        if (ImGui::SliderFloat2(label.c_str(), uv, -1.0f, 1.0f)) {
            this->vertex_[i].uv.x = uv[0];
            this->vertex_[i].uv.y = uv[1];
            this->is_change_vertex_ = true;
        }
    }

    static bool isRotation = false;
    if (ImGui::Checkbox("Rotation", &isRotation))imageFeatures |= FEATURE_ROTATE;
    else imageFeatures &= ~FEATURE_ROTATE;

    ImGui::End();
}