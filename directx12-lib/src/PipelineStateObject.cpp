#include "PipelineStateObject.h"
#include <stdexcept>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="psoDesc">psoの設定</param>
void PipelineStateObject::init(ID3D12Device* device)
{
    createGraphicsPipelineStateObject(device);
}

/// <summary>
/// グラフィックス用パイプラインステートオブジェクトの作成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="psoDesc">psoの設定</param>
/// <returns>
/// 生成したパイプラインステートオブジェクト
/// </returns>
void PipelineStateObject::createGraphicsPipelineStateObject(ID3D12Device* device)
{
    if (FAILED(device->CreateGraphicsPipelineState(&conf.desc, IID_PPV_ARGS(&this->pipelineStateObject)))) {
        throw std::runtime_error("failed to create pipeline state object");
    }
    return;
}