#pragma once
#include "RootSignature.h"
#include <memory>

/// <summary>
/// ルートシグネチャ ファクトリー クラス
/// </summary>
class RootSignatureFactory
{
public:
    /// <summary>
    /// ルートシグネチャを生成する
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="conf">生成に必要な設定</param>
    /// <returns>
    /// 生成&初期化されたルートシグネチャ
    /// </returns>
    static std::shared_ptr<RootSignature> create(ID3D12Device* device, const RootSignature::RootSignatureConf& conf)
    {
        std::shared_ptr<RootSignature> root_signature(new RootSignature(conf));
        root_signature->init(device);
        return root_signature;
    }
};
