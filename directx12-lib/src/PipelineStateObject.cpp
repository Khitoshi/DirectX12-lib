#include "PipelineStateObject.h"
#include <stdexcept>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="psoDesc">psoの設定</param>
void PipelineStateObject::init(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
    createGraphicsPipelineStateObject(device, psoDesc);
}

/// <summary>
/// グラフィックス用パイプラインステートオブジェクトの作成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="psoDesc">psoの設定</param>
/// <returns>
/// 生成したパイプラインステートオブジェクト
/// </returns>
void PipelineStateObject::createGraphicsPipelineStateObject(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
    if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&this->pipelineStateObject)))) {
        throw std::runtime_error("failed to create pipeline state object");
    }
    return;
}
