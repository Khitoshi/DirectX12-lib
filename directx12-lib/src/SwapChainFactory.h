#pragma once
#include "SwapChain.h"
#include <memory>

/// <summary>
/// スワップチェインを生成するファクトリクラス
/// </summary>
class SwapChainFactory
{
public:
    /// <summary>
    /// スワップチェインを生成する
    /// </summary>
    /// <param name="conf">設定</param>
    /// <returns>
    /// 生成&初期化されたスワップチェイン
    /// </returns>
    static std::shared_ptr<SwapChain> create(const SwapChain::SwapChainConf& conf)
    {
        std::shared_ptr<SwapChain> swap_chain(new SwapChain(conf));
        swap_chain->init();
        return swap_chain;
    }
};
