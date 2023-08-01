#pragma once

#include "d3dx12.h"

class RenderContext;

/// <summary>
/// ���z�I��scene
/// </summary>
class Scene
{
public:
    Scene() {};
    virtual ~Scene() {};

    /// <summary>
    /// ���z�ȏ�����
    /// </summary>
    /// <param name="conf"></param>
    virtual void init(ID3D12Device* device) = 0;

    /// <summary>
    /// ���z�ȏI����
    /// </summary>
    virtual void finalize() = 0;

    /// <summary>
    /// ���z�ȍX�V����
    /// </summary>
    virtual void update() = 0;

    /// <summary>
    /// ���z�ȕ`�揈��
    /// </summary>
    /// <param name="rc">�`�揈�������b�s���O���Ă���N���X�̃|�C���^</param>
    virtual void render(RenderContext* rc) = 0;

    /// <summary>
    /// ���z��ImGui���j���[
    /// </summary>
    virtual void updateImguiMenu() {};
};
