#include "SceneTriangle.h"

/// <summary>
/// ����������
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
/// �I��������
/// </summary>
void SceneTriangle::finalize()
{
}

/// <summary>
/// �X�V����
/// </summary>
void SceneTriangle::update()
{
}

/// <summary>
/// 3�p�`�`��
/// </summary>
/// <param name="conf"></param>
void SceneTriangle::render(SceneConf conf)
{
    triangle->draw(conf.renderContext);
}

