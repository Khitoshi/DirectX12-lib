#include "SceneTriangle.h"

void SceneTriangle::init()
{
    triangle = std::make_shared<Triangle>();
    TriangleConf triangleConf = {};
    triangleConf.device = conf.device;
    triangle->init(triangleConf);
}

void SceneTriangle::finalize()
{
}

void SceneTriangle::update()
{
}

void SceneTriangle::render()
{
    triangle->draw(conf.renderContext);
}

