#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;


class PipelineStateObject
{
public:
	PipelineStateObject() :pipelineStateObject(){};
	~PipelineStateObject() {};

	//初期化処理
	void init(ID3D12Device* device,D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);

private:
	//グラフィックスパイプラインステートオブジェクトの作成
	ComPtr<ID3D12PipelineState> createGraphicsPipelineStateObject(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);

public:
	//パイプラインステートオブジェクトの取得
	ID3D12PipelineState* getPipelineStateObject() const { return pipelineStateObject.Get(); } 

private:
	ComPtr<ID3D12PipelineState> pipelineStateObject;//パイプラインステートオブジェクト

};
