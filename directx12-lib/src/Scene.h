#pragma once

#include "d3dx12.h"
#include "RenderContext.h"

/// <summary>
/// シーン
/// </summary>
struct SceneConf {
    ID3D12Device* device;
    RenderContext* renderContext;
};

/// <summary>
/// 仮想的なscene
/// </summary>
class Scene
{
public:
    Scene() {};
    ~Scene() {};

    /// <summary>
    /// 仮想な初期化
    /// </summary>
    /// <param name="conf"></param>
    virtual void init(SceneConf conf) = 0;
    /// <summary>
    /// 仮想な終了か
    /// </summary>
    virtual void finalize() = 0;
    /// <summary>
    /// 仮想な更新処理
    /// </summary>
    virtual void update() = 0;
    /// <summary>
    /// 仮想な描画処理
    /// </summary>
    /// <param name="conf"></param>
    virtual void render(SceneConf conf) = 0;

    /// <summary>
    /// 仮想なImGuiメニュー
    /// </summary>
    virtual void updateImguiMenu() {};
};
