#pragma once
#include "Scene.h"
#include "Triangle.h"

/// <summary>
/// �O�p�`�`��p�V�[��
/// </summary>
class SceneTriangle : public Scene
{
public:
    SceneTriangle() {};
    ~SceneTriangle() {};

    //����������
    void init(SceneConf conf);
    //�I������
    void finalize();
    //�X�V����
    void update();
    //�`�揈��
    void render(SceneConf conf);

private:
    std::shared_ptr<Triangle> triangle;//�O�p�`
};
