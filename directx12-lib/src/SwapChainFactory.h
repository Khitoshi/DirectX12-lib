#pragma once
#include "SwapChain.h"
#include <memory>

/// <summary>
/// スワップチェインファクトリクラス
/// </summary>
class SwapChainFactory
{
public:
	static std::shared_ptr<SwapChain> create(const SwapChain::SwapChainConf& conf)
	{
		std::shared_ptr<SwapChain> swap_chain(new SwapChain(conf));
		swap_chain->init();
		return swap_chain;
	}
};
