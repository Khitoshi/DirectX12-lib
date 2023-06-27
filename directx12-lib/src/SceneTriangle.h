#pragma once
#include "Scene.h"
#include "Triangle.h"

/// <summary>
/// �O�p�`�`��p�V�[��
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

    //����������
    void init(SceneConf conf);
    //�I������
    void finalize();
    //�X�V����
    void update();
    //�`�揈��
    void render(SceneConf conf);
    //imgui��menu��\��
    void updateImguiMenu();
private:
    std::shared_ptr<Triangle> triangle;         //�O�p�`
    Triangle::Vertex vertex[3];                 //���_���W
    bool isChangeVertex;                        //���_���W��ύX������
};
