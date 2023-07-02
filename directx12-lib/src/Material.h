#pragma once
#include "Shader.h"
#include "Texture.h"
#include "PipelineStateObject.h"
#include "PSOCacheManager.h"
#include <memory>

class Material
{
public:

public:
    Material() {};
    ~Material() {};

    void setShaderPair(std::shared_ptr<BasicShaderPair> shader) { this->shader = shader; }
    void setVertexShader(std::shared_ptr<Shader> shader) { this->shader->vertexShader = shader; }
    void setPixelShader(std::shared_ptr<Shader> shader) { this->shader->pixelShader = shader; }

    void createPSO(ID3D12Device* device, PSOCacheManager::PSOParameters param)
    {

    }
private:
    std::shared_ptr<BasicShaderPair> shader;    // シェーダ
    std::shared_ptr<Texture> texture;           // テクスチャ
    std::shared_ptr<PipelineStateObject> pso;   // パイプラインステートオブジェクト
};
