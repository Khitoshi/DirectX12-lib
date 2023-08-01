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
        device_(nullptr),
        is_change_texture_(false)
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
    std::shared_ptr<Sprite> sprite_;    //�X�v���C�g
    Sprite::Vertex vertex_[4];          //���_���W
    bool is_change_vertex_;             //���_���W��ύX������

    ID3D12Device* device_;              //�f�o�C�X

    static std::string file_path_;      //�t�@�C���p�X
    bool is_change_texture_;            //�e�N�X�`����ύX������
};
