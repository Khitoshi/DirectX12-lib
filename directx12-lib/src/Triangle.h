#pragma once

#include "Shader.h"

class Triangle
{
public:
    Triangle();
    ~Triangle();

    void init();
private:
    //�V�F�[�_�[�̃��[�h
    void loadShader();

private:
    std::shared_ptr<Shader> vertexShader;//���_�V�F�[�_�[
    std::shared_ptr<Shader> pixelShader;//�s�N�Z���V�F�[�_�[
};
