#include "Rotation.h"

void Rotation::init()
{
    loadShader();
}

void Rotation::update(RenderContext* renderContext)
{
}

void Rotation::loadShader()
{
    //���_�V�F�[�_�[�̃��[�h
    this->shaderPair.vertexShader = std::make_shared<Shader>();
    ShaderConf vsConf = {};
    vsConf.filePath = "./src/shaders/RotationVS.hlsl";
    vsConf.entryFuncName = "VSMain";
    this->shaderPair.vertexShader->LoadVS(vsConf);

    //�s�N�Z���V�F�[�_�[�̃��[�h
    this->shaderPair.pixelShader = std::make_shared<Shader>();
    ShaderConf psConf = {};
    psConf.filePath = "./src/shaders/RotationPS.hlsl";
    psConf.entryFuncName = "PSMain";
    this->shaderPair.pixelShader->LoadPS(psConf);
}

void Rotation::initRootSignature()
{

}

void Rotation::initPipelineStateObject()
{
}
