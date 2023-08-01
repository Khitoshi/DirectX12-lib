#pragma once

#include "PipelineStateObject.h"
#include <memory>

/// <summary>
/// psoを作成するファクトリクラス
/// </summary>
class PipelineStateObjectFactory
{
public:
    /// <summary>
    /// psoを作成する
    /// </summary>
    /// <param name="conf">生成に関する設定</param>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化済みのpso
    /// </returns>
    static std::shared_ptr<PipelineStateObject> create(const PipelineStateObject::PipelineStateObjectConf& conf, ID3D12Device* device)
    {
        std::shared_ptr<PipelineStateObject> pso(new PipelineStateObject(conf));
        pso->init(device);
        return pso;
    }
};
