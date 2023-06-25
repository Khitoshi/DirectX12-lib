#include "SceneTriangle.h"

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::init(SceneConf conf)
{
    triangle = std::make_shared<Triangle>();
    TriangleConf triangleConf = {};
    triangleConf.device = conf.device;
    triangle->init(triangleConf);
}

/// <summary>
/// 終了化処理
/// </summary>
void SceneTriangle::finalize()
{
}

/// <summary>
/// 更新処理
/// </summary>
void SceneTriangle::update()
{
}

/// <summary>
/// 3角形描画
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::render(SceneConf conf)
{
    triangle->draw(conf.renderContext);
}

