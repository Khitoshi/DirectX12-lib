#pragma once
#include "OffScreenRenderTargetFactory.h"

#include <vector>
#include <stdexcept>

/// <summary>
/// オフスクリーンレンダーターゲットのキャッシュ管理システム
/// </summary>
class OffScreenRenderTargetCacheManager
{
private:
	OffScreenRenderTargetCacheManager() :
		off_screen_Render_target_list_()
	{};
	~OffScreenRenderTargetCacheManager() {};

public:
	static OffScreenRenderTargetCacheManager& getInstance()
	{
		static OffScreenRenderTargetCacheManager instance;
		return instance;
	}

	std::vector<OffScreenRenderTarget*> getRenderTargetList() const
	{
		return this->off_screen_Render_target_list_;
	}

	void addRenderTargetList(OffScreenRenderTarget* ort)
	{
		this->off_screen_Render_target_list_.push_back(ort);
	}

	void clearRenderTargetList()
	{
		this->off_screen_Render_target_list_.clear();
	}

	void reinit(ID3D12Device* device)
	{
		for (auto& rt : this->off_screen_Render_target_list_)
		{
			rt->createRenderTargetView(device);
		};
	}

private:
	std::vector<OffScreenRenderTarget*> off_screen_Render_target_list_;
};
