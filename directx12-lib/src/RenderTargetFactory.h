#pragma once
#include "RenderTarget.h"
#include <memory>

/// <summary>
/// �����_�[�^�[�Q�b�g�̃t�@�N�g���N���X
/// </summary>
class RenderTargetFactory
{
public:
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
};
