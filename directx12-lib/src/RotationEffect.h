#pragma once
#include "RootSignatureCacheManager.h"
#include "ShaderCacheManager.h"
#include "PSOCacheManager.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "RenderContext.h"
class RotationEffect
{
public:
    RotationEffect() {};
    ~RotationEffect() {};

    void init(ID3D12Device* device, const Camera* camera);
    void update(RenderContext* renderContext, const Camera* camera, VertexBuffer* vb, IndexBuffer* ib, int numIndices);

private:
    void createRootSignature(ID3D12Device* device);
    void createShader();
    void createPSO(ID3D12Device* device);
    void createConstantBuffer(ID3D12Device* device, const Camera* camera);

private:
    //頂点データ
    struct Vertex
    {
        DirectX::XMFLOAT3 position;	//座標
        DirectX::XMFLOAT2 uv;		//テクスチャ座標
    };

    Vertex vertices[4];										                        //頂点データ

    std::shared_ptr<RootSignature> rootSignature;
    BasicShaderPair shaderPair;
    std::shared_ptr<PipelineStateObject> pso;
    std::shared_ptr<ConstantBuffer> constantBuffer;

};
