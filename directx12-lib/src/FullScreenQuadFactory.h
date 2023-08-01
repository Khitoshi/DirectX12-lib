#pragma once
#include "FullScreenQuad.h"
#include <memory>

/// <summary>
/// フルスクリーン四角形の生成を行うクラス
/// </summary>
class FullScreenQuadFactory
{
public:
    /// <summary>
    /// フルスクリーン四角形を生成する
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <returns>
    /// 生成&初期化されたフルスクリーン四角形
    /// </returns>;
    static std::shared_ptr<FullScreenQuad> create(ID3D12Device* device)
    {
        std::shared_ptr<FullScreenQuad> quad(new FullScreenQuad());
        quad->init(device);
        return quad;
    }
};
