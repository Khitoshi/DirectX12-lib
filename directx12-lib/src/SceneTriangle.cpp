#include "SceneTriangle.h"
#include "imgui/imgui.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::init(SceneConf conf)
{
    //3角形のリソースを作成&初期化
    triangle = std::make_shared<Triangle>();
    TriangleConf triangleConf = {};
    triangleConf.device = conf.device;
    triangle->init(triangleConf);

    //頂点の初期値を設定
    this->vertex[0] = triangle->getVertices(0);
    this->vertex[1] = triangle->getVertices(1);
    this->vertex[2] = triangle->getVertices(2);
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
    if (isChangeVertex) {
        triangle->setVertices(vertex);
        isChangeVertex = false;
    }
}

/// <summary>
/// 3角形描画
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::render(SceneConf conf)
{
    triangle->draw(conf.renderContext);
}

/// <summary>
/// imguiのメニューを表示
/// </summary>
void SceneTriangle::updateImguiMenu()
{
	ImGui::Begin("Triangle");

    //レンダーモードの変更
    if (ImGui::Button("Solid")) {
        triangle->setRenderMode(RenderMode::Solid);
    }
    else if (ImGui::Button("WireFrame")) {
        triangle->setRenderMode(RenderMode::WireFrame);
    }


    //頂点の位置
    for (int i = 0; i < 3; ++i) {
        float position[3] = { vertex[i].position.x, vertex[i].position.y, vertex[i].position.z };
        std::string label = "Vertex pos" + std::to_string(i + 1);

        if (ImGui::SliderFloat3(label.c_str(), position, -1.0f, 1.0f)) {
            vertex[i].position.x = position[0];
            vertex[i].position.y = position[1];
            vertex[i].position.z = position[2];
            isChangeVertex = true;
        }
    }


    //頂点の色
    for (int i = 0; i < 3; ++i) {
        float color[4] = { vertex[i].color.x, vertex[i].color.y, vertex[i].color.z, vertex[i].color.w };
        std::string label = "Vertex color" + std::to_string(i + 1);

        if (ImGui::ColorEdit4(label.c_str(), color)) {
            vertex[i].color.x = color[0];
            vertex[i].color.y = color[1];
            vertex[i].color.z = color[2];
            vertex[i].color.w = color[3];
            isChangeVertex = true;
        }
    }

	ImGui::End();
}

