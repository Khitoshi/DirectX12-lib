#pragma once
#include "Sprite.h"
#include "Scene.h"
#include <memory>

/// <summary>
/// �X�v���C�g�`��p�V�[��
/// </summary>
class SceneSprite : public Scene
{
public:
    SceneSprite() :
        sprite(),
        vertex(),
        isChangeVertex(false)
    {};
    ~SceneSprite() {};

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
    enum ImageFeature
    {
        FEATURE_NONE = 0,
        FEATURE_ROTATE = 1 << 0, // 0001
        //FEATURE_SCALE = 1 << 1,  // 0010
        //FEATURE_TRANSLATE = 1 << 2 // 0100
        // �X�ɒǉ��@�\������΂����ɒ�`���܂�...
    };
    unsigned int imageFeatures = FEATURE_NONE;

    std::shared_ptr<Sprite> sprite;         //�X�v���C�g
    Sprite::Vertex vertex[4];               //���_���W
    bool isChangeVertex;                    //���_���W��ύX������
};
