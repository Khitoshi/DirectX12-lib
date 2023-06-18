#include "Triangle.h"

void Triangle::init()
{






}

void Triangle::loadShader()
{
    vertexShader = std::make_shared<Shader>();
    vertexShader->LoadVS("./shaders/triangle.hlsl", "VSMain");

    pixelShader = std::make_shared<Shader>();
    pixelShader->LoadPS("./shaders/triangle.hlsl", "PSMain");
}
