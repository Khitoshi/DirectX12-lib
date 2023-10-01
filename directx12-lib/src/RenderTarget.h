#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>
#include "Descriptor.h"

class DescriptorHeap;

using namespace Microsoft::WRL;

class RenderTarget :public Descriptor
{
	friend class RenderTargetFactory;
private:
	RenderTarget() :
		Descriptor(Descriptor::DescriptorType::RenderTarget) {};

public:
	~RenderTarget() {};

private:
	//���C���`��p�����_�[�^�[�Q�b�g���쐬
	void init(ID3D12Device* device, IDXGISwapChain3* swap_chain, const UINT& buffer, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);

	//�I�t�X�N���[���p�����_�[�^�[�Q�b�g���쐬
	void init(ID3D12Device* device, const D3D12_RESOURCE_STATES& initial_state, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);

public://�擾�n

	CD3DX12_RESOURCE_BARRIER begin();
	CD3DX12_RESOURCE_BARRIER end();

	D3D12_RESOURCE_DESC getDesc();
};
