#pragma once
#include "Sprite.h"
#include "Scene.h"
#include <memory>

class SceneSprite : public Scene
{
public:
    SceneSprite(){};
    ~SceneSprite(){};

    //����������
    void init(SceneConf conf);
    //�I������
    void finalize();
    //�X�V����
    void update();
    //�`�揈��
    void render(SceneConf conf);
    //ImGui���j���[
    void updateImguiMenu();
private:
    std::shared_ptr<Sprite> sprite;
};
