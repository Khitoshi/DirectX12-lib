#include "SceneTriangle.h"
#include "imgui/imgui.h"

/// <summary>
/// ����������
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::init(SceneConf conf)
{
    //3�p�`�̃��\�[�X���쐬&������
    triangle = std::make_shared<Triangle>();
    TriangleConf triangleConf = {};
    triangleConf.device = conf.device;
    triangle->init(triangleConf);

    //���_�̏����l��ݒ�
    this->vertex[0] = triangle->getVertices(0);
    this->vertex[1] = triangle->getVertices(1);
    this->vertex[2] = triangle->getVertices(2);
}

/// <summary>
/// �I��������
/// </summary>
void SceneTriangle::finalize()
{
}

/// <summary>
/// �X�V����
/// </summary>
void SceneTriangle::update()
{
    if (isChangeVertex) {
        triangle->setVertices(vertex);
        isChangeVertex = false;
    }
}

/// <summary>
/// 3�p�`�`��
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::render(SceneConf conf)
{
    triangle->draw(conf.renderContext);
}

/// <summary>
/// imgui�̃��j���[��\��
/// </summary>
void SceneTriangle::updateImguiMenu()
{
	ImGui::Begin("Triangle");

    //�����_�[���[�h�̕ύX
    if (ImGui::Button("Solid")) {
        triangle->setRenderMode(RenderMode::Solid);
    }
    else if (ImGui::Button("WireFrame")) {
        triangle->setRenderMode(RenderMode::WireFrame);
    }


    //���_�̈ʒu
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


    //���_�̐F
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

