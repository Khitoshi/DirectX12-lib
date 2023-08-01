#include "PipelineStateObject.h"
#include <stdexcept>

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">デバイス</param>
void PipelineStateObject::init(ID3D12Device* device)
{
    if (FAILED(device->CreateGraphicsPipelineState(&conf_.desc, IID_PPV_ARGS(&this->pso_)))) {
        throw std::runtime_error("failed to create pipeline state object");
    }
}