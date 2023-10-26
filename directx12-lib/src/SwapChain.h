#pragma once

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <stdexcept>

using namespace Microsoft::WRL;

/// <summary>
/// スワップチェイン
/// </summary>
class SwapChain
{
	friend class SwapChainFactory;
public:
	struct SwapChainConf {
		UINT frame_buffer_count;
		UINT width;
		UINT height;
		HWND hWnd;
		IDXGIFactory4* factory;
		ID3D12CommandQueue* command_queue;
	};

private:
	SwapChain(const SwapChainConf& c) :
		conf_(c),
		swap_chain_(),
		desc_(),
		current_back_buffer_index_(0)
	{};

public:
	~SwapChain() {};

	void present();

	void resizeBuffer(const UINT& width, const UINT& height);
	void setFullScreen(bool is_full_screen);
	void resizeTarget(DXGI_MODE_DESC& desc);

private:
	void init();
	void createSwapChain();
	void createCurrentBackBufferIndex();

public:
	IDXGISwapChain3* getSwapChain() const { return swap_chain_.Get(); }
	UINT getCurrentBackBufferIndex() const { return this->swap_chain_->GetCurrentBackBufferIndex(); }
	bool isFullScreen() const {
		BOOL fullscreen_state;
		if (FAILED(this->swap_chain_->GetFullscreenState(&fullscreen_state, nullptr))) {
			throw std::runtime_error("フルスクリーン状態の取得に失敗しました");
		}
		return fullscreen_state;
	}
	DXGI_SWAP_CHAIN_DESC1 getDesc() const { return this->desc_; }

private:
	SwapChainConf conf_;
	ComPtr<IDXGISwapChain3> swap_chain_;
	DXGI_SWAP_CHAIN_DESC1 desc_;
	UINT current_back_buffer_index_;

};
