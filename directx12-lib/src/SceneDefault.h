#pragma once

#include "Scene.h"

/// <summary>
/// ���̏��������Ȃ��V�[��
/// </summary>
class SceneDefault: public Scene
{
public:
    SceneDefault() {};
    ~SceneDefault() {};

    //����������
    void init(SceneConf conf) {};
    //�I������
    void finalize() {};
    //�X�V����
    void update() {};
    //�`�揈��
    void render(SceneConf conf) {};
};
