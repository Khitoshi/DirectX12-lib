#pragma once

#include "PipelineStateObject.h"
#include <memory>

/// <summary>
/// psoを作成するファクトリクラス
/// </summary>
class PipelineStateObjectFactory
{
public:
	static std::shared_ptr<PipelineStateObject> create(const PipelineStateObject::PipelineStateObjectConf& conf, ID3D12Device* device)
	{
		std::shared_ptr<PipelineStateObject> pso(new PipelineStateObject(conf));
		pso->init(device);
		return pso;
	}
};
