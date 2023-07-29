#pragma once

#include "d3dx12.h"

class RenderContext;

/// <summary>
/// 仮想的なscene
/// </summary>
class Scene
{
public:
    Scene() {};
    virtual ~Scene() {};

    /// <summary>
    /// 仮想な初期化
    /// </summary>
    /// <param name="conf"></param>
    virtual void init(ID3D12Device* device) = 0;

    /// <summary>
    /// 仮想な終了化
    /// </summary>
    virtual void finalize() = 0;

    /// <summary>
    /// 仮想な更新処理
    /// </summary>
    virtual void update() = 0;

    /// <summary>
    /// 仮想な描画処理
    /// </summary>
    /// <param name="rc">描画処理をラッピングしているクラスのポインタ</param>
    virtual void render(RenderContext* rc) = 0;

    /// <summary>
    /// 仮想なImGuiメニュー
    /// </summary>
    virtual void updateImguiMenu() {};
};
