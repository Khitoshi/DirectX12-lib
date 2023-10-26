#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// パイプラインステートオブジェクトファクトリクラス
/// </summary>
class PipelineStateObject
{
	friend class PipelineStateObjectFactory;
public:
	struct PipelineStateObjectConf {
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;

		bool operator==(const PipelineStateObjectConf& other) const {
			return memcmp(this, &other, sizeof(PipelineStateObjectConf)) == 0;
		}
	};

private:
	PipelineStateObject(const PipelineStateObjectConf& c) :
		conf_(c),
		pso_()
	{};

public:
	~PipelineStateObject() {};

private:
	void init(ID3D12Device* device);

public:
	ID3D12PipelineState* getPipelineStateObject() const { return pso_.Get(); }

private:
	PipelineStateObjectConf conf_;
	ComPtr<ID3D12PipelineState> pso_;
};
