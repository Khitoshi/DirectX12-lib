#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;


/// <summary>
/// パイプラインステートオブジェクトの生成用クラス
/// </summary>
class PipelineStateObject
{
public:
    /// <summary>
    /// パイプラインステートオブジェクト生成時に使用する設定
    /// </summary>
    struct PipelineStateObjectConf {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;

        bool operator==(const PipelineStateObjectConf& other) const {
			return memcmp(this, &other, sizeof(PipelineStateObjectConf)) == 0;
		}
    };

public:
    PipelineStateObject(PipelineStateObjectConf c) :pipelineStateObject(),conf(c) {};
    ~PipelineStateObject() {};

    //初期化処理
    void init(ID3D12Device* device);

private:
    //グラフィックスパイプラインステートオブジェクトの作成
    void createGraphicsPipelineStateObject(ID3D12Device* device);

public:
    //パイプラインステートオブジェクトの取得
    ID3D12PipelineState* getPipelineStateObject() const { return pipelineStateObject.Get(); }

private:
    ComPtr<ID3D12PipelineState> pipelineStateObject;    //パイプラインステートオブジェクト
    PipelineStateObjectConf conf;
};
