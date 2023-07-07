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

	void init(ID3D12Device* device,const Camera const* camera);
	void update(RenderContext* renderContext, const Camera const* camera);

private:
	void createRootSignature(ID3D12Device* device);
	void createShader();
	void createPSO(ID3D12Device* device);
	void createConstantBuffer(ID3D12Device* device, const Camera const* camera);
private:
	std::shared_ptr<RootSignature> rootSignature;
	BasicShaderPair shaderPair;
	std::shared_ptr<PipelineStateObject> pso;
	std::shared_ptr<ConstantBuffer> constantBuffer;
};
