#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// パイプラインステートオブジェクト生成時に使用する設定
/// </summary>
struct PipelineStateObjectConf {
    ID3D12Device* device;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
};

/// <summary>
/// パイプラインステートオブジェクトの生成用クラス
/// </summary>
class PipelineStateObject
{
public:
    PipelineStateObject() :pipelineStateObject() {};
    ~PipelineStateObject() {};

    //初期化処理
    void init(PipelineStateObjectConf conf);

private:
    //グラフィックスパイプラインステートオブジェクトの作成
    void createGraphicsPipelineStateObject(PipelineStateObjectConf conf);

public:
    //パイプラインステートオブジェクトの取得
    ID3D12PipelineState* getPipelineStateObject() const { return pipelineStateObject.Get(); }

private:
    ComPtr<ID3D12PipelineState> pipelineStateObject;    //パイプラインステートオブジェクト
};
