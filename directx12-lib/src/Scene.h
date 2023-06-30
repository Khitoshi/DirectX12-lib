#pragma once

#include "d3dx12.h"
#include "RenderContext.h"

/// <summary>
/// �V�[��
/// </summary>
struct SceneConf {
    ID3D12Device* device;
    RenderContext* renderContext;
};

/// <summary>
/// ���z�I��scene
/// </summary>
class Scene
{
public:
    Scene() {};
    ~Scene() {};

    /// <summary>
    /// ���z�ȏ�����
    /// </summary>
    /// <param name="conf"></param>
    virtual void init(SceneConf conf) = 0;
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
    /// <param name="conf"></param>
    virtual void render(SceneConf conf) = 0;

    /// <summary>
    /// ���z��ImGui���j���[
    /// </summary>
    virtual void updateImguiMenu() {};
};
