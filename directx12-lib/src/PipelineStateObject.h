#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// パイプラインステートオブジェクトの生成用クラス
/// </summary>
class PipelineStateObject
{
    friend class PipelineStateObjectFactory;
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

private:
    PipelineStateObject(const PipelineStateObjectConf& c) :
        conf_(c),
        pso_()
    {};

public:
    ~PipelineStateObject() {};

private:
    //初期化処理
    void init(ID3D12Device* device);

public:
    //パイプラインステートオブジェクトの取得
    ID3D12PipelineState* getPipelineStateObject() const { return pso_.Get(); }

private:
    PipelineStateObjectConf conf_;      //パイプラインステートオブジェクト生成時に使用する設定
    ComPtr<ID3D12PipelineState> pso_;   //パイプラインステートオブジェクト
};
