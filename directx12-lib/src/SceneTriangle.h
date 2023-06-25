#pragma once
#include "Triangle.h"

struct SceneConf {
    ID3D12Device* device;
    RenderContext* renderContext;
};

class SceneTriangle
{
public:
    SceneTriangle(const SceneConf c) :conf(c) {};
    ~SceneTriangle() {};

    void init();
    void finalize();
    void update();
    void render();

private:
    std::shared_ptr<Triangle> triangle;
    SceneConf conf;
};
