#pragma once

#include "Scene.h"

/// <summary>
/// ���̏��������Ȃ��V�[��
/// </summary>
class SceneDefault final : public Scene
{
public:
    SceneDefault() {};
    ~SceneDefault() {};

    //����������
    void init(ID3D12Device* device)override {};
    //�I������
    void finalize()override {};
    //�X�V����
    void update()override {};
    //�`�揈��
    void render(RenderContext* rc)override {};
};
