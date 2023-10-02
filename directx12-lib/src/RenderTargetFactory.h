#pragma once
#include "RenderTarget.h"
#include <memory>

/// <summary>
/// レンダーターゲットを生成するクラス
/// </summary>
class RenderTargetFactory
{
public:
	//メインで使用するレンダーターゲットを作成
	static std::shared_ptr<RenderTarget> create(
		ID3D12Device* device,
		IDXGISwapChain3* swap_chain,
		const UINT& buffer,
		const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
	{
		std::shared_ptr<RenderTarget> render_target(new RenderTarget(swap_chain, buffer, handle));
		render_target->init(device);
		return render_target;
	}

	//オフスクリーン用等のレンダーターゲットを作成
	static std::shared_ptr<RenderTarget> create(
		ID3D12Device* device,
		const D3D12_RESOURCE_STATES& status,
		const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
	{
		std::shared_ptr<RenderTarget> render_target(new RenderTarget(handle, status));
		render_target->init(device);
		return render_target;
	}

};
