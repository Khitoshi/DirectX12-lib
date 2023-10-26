#pragma once
#include "CompositeRenderTarget.h"
#include <memory>

/// <summary>
/// �I�t�X�N���[����1���̃e�N�X�`���ɍ�������N���X�̃t�@�N�g���N���X
/// </summary>
class CompositeRenderTargetFactory
{
public:
	static std::shared_ptr<CompositeRenderTarget> create(const CompositeRenderTarget::CompositeRenderTargetConf& crt_conf, ID3D12Device* device)
	{
		std::shared_ptr<CompositeRenderTarget> compositeRenderTarget(new CompositeRenderTarget(crt_conf));
		compositeRenderTarget->init(device);
		return compositeRenderTarget;
	}
};
