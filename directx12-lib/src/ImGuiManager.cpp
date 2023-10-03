#include "ImGuiManager.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "GraphicsConfigurator.h"
#include "OffScreenRenderTargetFactory.h"
#include "DepthStencilCacheManager.h"
#include "RenderContext.h"
#include "DescriptorHeapFactory.h"
#include <stdexcept>

/// <summary>
/// imguiの初期化
/// </summary>
/// <param name="device">GPUデバイス</param>
/// <param name="hWnd">ウィンドウハンドルのインターフェース</param>
void ImGuiManager::init(ID3D12Device* device)
{
	createDescriptorHeap(device);
	createOffScreenRenderTarget(device);
	createDepthStencil(device);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(*this->hWnd_);
	ImGui_ImplDX12_Init(
		device,
		GraphicsConfigurator::getFrameBufferCount(),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		this->descriptor_heap_->getDescriptorHeap(),
		this->descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
}

/// <summary>
/// Frame開始処理
/// </summary>
void ImGuiManager::beginFrame(RenderContext* rc, ID3D12Device* device)
{
	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

/// <summary>
/// Frame終了処理
/// </summary>
void ImGuiManager::endFrame()
{
	ImGui::Render();
}

/// <summary>
/// imguiの描画
/// </summary>
/// <param name="rc">レンダーコンテキスト</param>
/// <param name="device">GPUデバイス</param>
void ImGuiManager::render(RenderContext* rc, ID3D12Device* device)
{
	//TODO:以下のifでは
	// this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	//で複数回simplestartを呼び出しているので、えらーが　発生しているので別の方法を考える
	//しかしimguiのボタンでウィンドウサイズの変更を行っているのでbeginrenderで行うことができていないということがある

	if (this->off_screen_render_target_->getResource() == nullptr) {
		deinit();
		init(device);
		//beginFrame(rc, device);
	}

	ImGui::Render();
	rc->setDescriptorHeap(this->descriptor_heap_.get());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), rc->getCommandList());

	//オフスクリーンレンダーターゲットの書き込みを終了する。
	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(this->off_screen_render_target_.get());
}

/// <summary>
/// 解放処理
/// </summary>
void ImGuiManager::deinit()
{
	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

/// <summary>
/// ディスクリプタヒープの作成
/// </summary>
/// <param name="device">GPUデバイス</param>
void ImGuiManager::createDescriptorHeap(ID3D12Device* device)
{
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

void ImGuiManager::createOffScreenRenderTarget(ID3D12Device* device)
{
	//TODO:リファクタリング対象
	OffScreenRenderTarget::OffScreenRenderTargetConf osrt_conf = {};
	{//ディスクリプタヒープの設定
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		osrt_conf.descriptor_heap_desc = desc;
	}
	{//リソースデスクの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = GraphicsConfigurator::getWindowWidth();
		desc.Height = GraphicsConfigurator::getWindowHeight();
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		osrt_conf.resource_desc = desc;
	}

	this->off_screen_render_target_ = OffScreenRenderTargetFactory::create(osrt_conf, device);
}

void ImGuiManager::createDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = GraphicsConfigurator::getFrameBufferCount();
	ds_conf.width = GraphicsConfigurator::getWindowWidth();
	ds_conf.height = GraphicsConfigurator::getWindowHeight();
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}