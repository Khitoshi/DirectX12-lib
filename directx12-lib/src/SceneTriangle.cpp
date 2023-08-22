#include "SceneTriangle.h"
#include <imgui/imgui.h>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::init(ID3D12Device* device)
{
    //3角形のリソースを作成&初期化
    this->triangle_ = std::make_shared<Triangle>();
    this->triangle_->init(device);

    //頂点の初期値を設定
    this->vertex_[0] = this->triangle_->getVertices(0);
    this->vertex_[1] = this->triangle_->getVertices(1);
    this->vertex_[2] = this->triangle_->getVertices(2);
}

/// <summary>
/// 終了化処理
/// </summary>
void SceneTriangle::finalize()
{
}

/// <summary>
/// 更新処理
/// </summary>
void SceneTriangle::update()
{
    if (this->is_change_vertex_) {
        this->triangle_->setVertices(this->vertex_);
        this->is_change_vertex_ = false;
    }
}

/// <summary>
/// 3角形描画
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::render(RenderContext* rc)
{
    this->triangle_->draw(rc);
}

/// <summary>
/// imguiのメニューを表示
/// </summary>
void SceneTriangle::updateImguiMenu()
{
    ImGui::Begin("Triangle");

    //レンダーモードの変更
    if (ImGui::Button("Solid")) {
        this->triangle_->setRenderMode(RenderMode::Solid);
    }
    if (ImGui::Button("WireFrame")) {
        this->triangle_->setRenderMode(RenderMode::WireFrame);
    }

    //頂点の位置
    for (int i = 0; i < 3; ++i) {
        float position[3] = { this->vertex_[i].position.x, this->vertex_[i].position.y, this->vertex_[i].position.z };
        std::string label = "Vertex pos" + std::to_string(i + 1);

        if (ImGui::SliderFloat3(label.c_str(), position, -1.0f, 1.0f)) {
            this->vertex_[i].position.x = position[0];
            this->vertex_[i].position.y = position[1];
            this->vertex_[i].position.z = position[2];
            this->is_change_vertex_ = true;
        }
    }

    //頂点の色
    for (int i = 0; i < 3; ++i) {
        float color[4] = { this->vertex_[i].color.x, this->vertex_[i].color.y, this->vertex_[i].color.z, this->vertex_[i].color.w };
        std::string label = "Vertex color" + std::to_string(i + 1);

        if (ImGui::ColorEdit4(label.c_str(), color)) {
            this->vertex_[i].color.x = color[0];
            this->vertex_[i].color.y = color[1];
            this->vertex_[i].color.z = color[2];
            this->vertex_[i].color.w = color[3];
            this->is_change_vertex_ = true;
        }
    }

    ImGui::End();
}