#pragma once

#include "Scene.h"
#include "Sprite.h"
#include <memory>

class Camera;

/// <summary>
/// �X�v���C�g�`��p�V�[��
/// </summary>
class SceneSprite final : public Scene
{
public:
    SceneSprite() :
        sprite_(),
        vertex_(),
        is_change_vertex_(false),
        camera_()
    {};
    ~SceneSprite() {};

    //����������
    void init(ID3D12Device* device)override;
    //�I������
    void finalize()override;
    //�X�V����
    void update()override;
    //�`�揈��
    void render(RenderContext* rc)override;
    //ImGui���j���[
    void updateImguiMenu()override;
private:
    enum ImageFeature
    {
        FEATURE_NONE = 0,
        FEATURE_ROTATE = 1 << 0, // 0001
        //FEATURE_SCALE = 1 << 1,  // 0010
        //FEATURE_TRANSLATE = 1 << 2 // 0100

    };
    unsigned int imageFeatures = FEATURE_NONE;

    std::shared_ptr<Sprite> sprite_;    //�X�v���C�g
    Sprite::Vertex vertex_[4];          //���_���W
    bool is_change_vertex_;             //���_���W��ύX������
    std::shared_ptr<Camera> camera_;    //�J����
};
