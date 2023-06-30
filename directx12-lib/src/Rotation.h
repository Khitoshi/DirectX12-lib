#include "Shader.h"
#include "RenderContext.h"
#include "PipelineStateObject.h"
class Rotation
{
public:
	struct RotationConf
	{
		ID3D12Device* device;
	};
public:
	Rotation(RotationConf c):conf(c) {};
	~Rotation() {};

	void init();
	void update(RenderContext* renderContext);
private:
	void loadShader();
	void initRootSignature();
	void initPipelineStateObject();
private:
	RotationConf conf;
	BasicShaderPair shaderPair;
	std::shared_ptr<PipelineStateObject> pipelineStateObject;
	ComPtr<ID3D12RootSignature> rootSignature;
};
