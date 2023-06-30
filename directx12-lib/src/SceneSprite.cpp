#include "SceneSprite.h"
#include "imgui/imgui.h"

void SceneSprite::init(SceneConf conf)
{
    //3角形のリソースを作成&初期化
    sprite = std::make_shared<Sprite>();
    Sprite::SpriteConf spriteConf = {};
    spriteConf.device = conf.device;
    spriteConf.filePath = "asset/img/test_image.png";
    sprite->init(spriteConf);

    //頂点座標を設定
    vertex[0] = sprite->getVertices(0);
    vertex[1] = sprite->getVertices(1);
    vertex[2] = sprite->getVertices(2);
    vertex[3] = sprite->getVertices(3);
}

void SceneSprite::finalize()
{
}

void SceneSprite::update()
{
    /*
    switch (imageFeatures)
    {
    case FEATURE_NONE:
    case FEATURE_ROTATE:
    };
    */

    if (isChangeVertex) {
		sprite->setVertices(vertex);
		isChangeVertex = false;
	}
}

void SceneSprite::render(SceneConf conf)
{
    sprite->draw(conf.renderContext);
}

void SceneSprite::updateImguiMenu()
{
    ImGui::Begin("Sprite");

    //頂点の位置
    for (int i = 0; i < 4; ++i) {
        float position[3] = { vertex[i].position.x, vertex[i].position.y, vertex[i].position.z };
        std::string label = "Vertex pos" + std::to_string(i + 1);

        if (ImGui::SliderFloat3(label.c_str(), position, -1.0f, 1.0f)) {
            vertex[i].position.x = position[0];
            vertex[i].position.y = position[1];
            vertex[i].position.z = position[2];
            isChangeVertex = true;
        }
    }

    //頂点の位置
    for (int i = 0; i < 4; ++i) {
        float uv[2] = { vertex[i].uv.x, vertex[i].uv.y,};
        std::string label = "Vertex uv" + std::to_string(i + 1);

        if (ImGui::SliderFloat2(label.c_str(), uv, -1.0f, 1.0f)) {
            vertex[i].uv.x = uv[0];
            vertex[i].uv.y = uv[1];
            isChangeVertex = true;
        }
    }
        
    static bool isRotation = false;
    if (ImGui::Checkbox("Rotation", &isRotation))imageFeatures |= FEATURE_ROTATE;
	else imageFeatures &= ~FEATURE_ROTATE;

	ImGui::End();
}
