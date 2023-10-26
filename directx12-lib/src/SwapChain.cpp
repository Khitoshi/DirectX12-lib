#include "SwapChain.h"
#include <stdexcept>
#include <comdef.h>
#include "GraphicsConfigurator.h"

void SwapChain::init()
{
	createSwapChain();
	createCurrentBackBufferIndex();

	this->swap_chain_->GetDesc1(&this->desc_);
}

void SwapChain::present()
{
	if (FAILED(this->swap_chain_->Present(1, 0))) {
		throw std::runtime_error("スワップチェインのプレゼントに失敗しました");
	}

	this->current_back_buffer_index_ = this->swap_chain_->GetCurrentBackBufferIndex();
}

void SwapChain::resizeBuffer(const UINT& width, const UINT& height)
{
	DXGI_SWAP_CHAIN_DESC desc = {};
	this->swap_chain_->GetDesc(&desc);

	if (FAILED(this->swap_chain_->ResizeBuffers(this->conf_.frame_buffer_count, width, height, desc.BufferDesc.Format, desc.Flags))) {
		throw std::runtime_error("スワップチェインのリサイズに失敗しました");
	}

}

void SwapChain::resizeTarget(DXGI_MODE_DESC& desc)
{
	if (FAILED(this->swap_chain_->ResizeTarget(&desc))) {
		throw std::runtime_error("スワップチェインのリサイズに失敗しました");
	}
}

void SwapChain::createSwapChain()
{
	//スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = this->conf_.frame_buffer_count;
	swapChainDesc.Width = this->conf_.width;
	swapChainDesc.Height = this->conf_.height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	//スワップチェイン1生成
	ComPtr<IDXGISwapChain1> swap_chain1;
	if (FAILED(this->conf_.factory->CreateSwapChainForHwnd(
		this->conf_.command_queue,
		this->conf_.hWnd,
		//&swapChainDesc,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swap_chain1
	))) {
		throw std::runtime_error("スワップチェイン1の生成に失敗しました");
	}

	if (FAILED(swap_chain1->QueryInterface(IID_PPV_ARGS(&this->swap_chain_)))) {
		throw std::runtime_error("スワップチェイン3のインターフェースの取得に失敗しました");
	}

}

void SwapChain::createCurrentBackBufferIndex()
{
	this->current_back_buffer_index_ = this->swap_chain_->GetCurrentBackBufferIndex();
}

void SwapChain::setFullScreen(bool toFullScreen)
{
	if (toFullScreen) {
		ComPtr<IDXGIOutput> output;
		this->swap_chain_->GetContainingOutput(&output);
		if (output)
		{
			DXGI_OUTPUT_DESC desc{};
			output->GetDesc(&desc);
		}

		this->swap_chain_->SetFullscreenState(TRUE, /*output.Get()*/nullptr);
	}
	else
	{
		this->swap_chain_->SetFullscreenState(FALSE, nullptr);
	}
}