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
    //頂点シェーダーのロード
    this->shaderPair.vertexShader = std::make_shared<Shader>();
    ShaderConf vsConf = {};
    vsConf.filePath = "./src/shaders/RotationVS.hlsl";
    vsConf.entryFuncName = "VSMain";
    vsConf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
    this->shaderPair.vertexShader->load(vsConf);

    //ピクセルシェーダーのロード
    this->shaderPair.pixelShader = std::make_shared<Shader>();
    ShaderConf psConf = {};
    psConf.filePath = "./src/shaders/RotationPS.hlsl";
    psConf.entryFuncName = "PSMain";
    psConf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
    this->shaderPair.pixelShader->load(psConf);
}

void Rotation::initRootSignature()
{

}

void Rotation::initPipelineStateObject()
{
}
