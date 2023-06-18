#pragma once

#include "Shader.h"

class Triangle
{
public:
    Triangle();
    ~Triangle();

    void init();
private:
    //シェーダーのロード
    void loadShader();

private:
    std::shared_ptr<Shader> vertexShader;//頂点シェーダー
    std::shared_ptr<Shader> pixelShader;//ピクセルシェーダー
};
