#include "PipelineStateObject.h"
#include <stdexcept>

/// <summary>
/// ����������
/// </summary>
/// <param name="device">�f�o�C�X</param>
void PipelineStateObject::init(ID3D12Device* device)
{
    if (FAILED(device->CreateGraphicsPipelineState(&conf_.desc, IID_PPV_ARGS(&this->pso_)))) {
        throw std::runtime_error("failed to create pipeline state object");
    }
}