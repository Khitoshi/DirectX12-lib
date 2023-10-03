#include "RenderTarget.h"
#include "DescriptorHeapFactory.h"
#include "Descriptor.h"
#include "GraphicsConfigurator.h"
#include <stdexcept>

void RenderTarget::init(ID3D12Device* device)
{
	if (swap_chain_) {//スワップチェインがある場合
		if (FAILED(swap_chain_->GetBuffer(this->buffer_, IID_PPV_ARGS(getResourceAddress())))) {
			throw std::runtime_error("FAILED Descriptor::createCommittedResource GetBuffer");
		}

		//device->CreateRenderTargetView(getResource(), nullptr, this->handle_);
	}
	else {//TODO:汚いのでリファクタリングする
		//レンダリングターゲットビューの作成
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Height = GraphicsConfigurator::getWindowHeight();
		desc.Width = GraphicsConfigurator::getWindowWidth();
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		D3D12_CLEAR_VALUE clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, GraphicsConfigurator::getBackgroundColor());

		createCommittedResource(device, prop, D3D12_HEAP_FLAG_NONE, desc, this->resouce_status_, &clear_value);
	}

	device->CreateRenderTargetView(getResource(), nullptr, this->handle_);
	setName(L"RenderTarget");
}

CD3DX12_RESOURCE_BARRIER RenderTarget::begin()
{
	return CD3DX12_RESOURCE_BARRIER::Transition(
		getResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
}

CD3DX12_RESOURCE_BARRIER RenderTarget::end()
{
	return CD3DX12_RESOURCE_BARRIER::Transition(
		getResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
}

D3D12_RESOURCE_DESC RenderTarget::getDesc()
{
	return getResource()->GetDesc();
}
