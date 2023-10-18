#include "ImGuiManager.h"
#include "OffScreenRenderTargetCacheManager.h"
#include "GraphicsConfigurator.h"
#include "OffScreenRenderTargetFactory.h"
#include "RootSignatureCacheManager.h"
#include "DepthStencilCacheManager.h"
#include "RenderContext.h"
#include "DescriptorHeapFactory.h"
#include <stdexcept>

void ImGuiManager::init(ID3D12Device* device)
{
	createDescriptorHeap(device);
	createOffScreenRenderTarget(device);
	createRootSignature(device);
	createDepthStencil(device);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(*this->hWnd_);
	ImGui_ImplDX12_Init(
		device,
		GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		this->descriptor_heap_->getDescriptorHeap(),
		this->descriptor_heap_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		this->descriptor_heap_->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::beginRender(RenderContext* rc, ID3D12Device* device)
{
	if (this->off_screen_render_target_->getResource() == nullptr) {
		deinit();
		init(device);
	}

	this->off_screen_render_target_->beginRender(rc, this->depth_stencil_->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	rc->setDescriptorHeap(this->descriptor_heap_.get());
	rc->setRootSignature(this->root_signature_.get());

	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void ImGuiManager::endRender(RenderContext* rc, ID3D12Device* device)
{
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), rc->getCommandList());

	//オフスクリーンレンダーターゲットの書き込みを終了する。
	this->off_screen_render_target_->endRender(rc);
	OffScreenRenderTargetCacheManager::getInstance().addRenderTargetList(this->off_screen_render_target_.get());
}

void ImGuiManager::deinit()
{
	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::createDescriptorHeap(ID3D12Device* device)
{
	this->descriptor_heap_ = DescriptorHeapFactory::create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 16);
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
		desc.Width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
		desc.Height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
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

void ImGuiManager::createRootSignature(ID3D12Device* device)
{
	RootSignature::RootSignatureConf rootSignatureConf = {};
	rootSignatureConf.sampler_filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	rootSignatureConf.texture_address_modeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.texture_address_modeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	rootSignatureConf.root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureConf.num_srv = 2;
	rootSignatureConf.visibility_cbv = D3D12_SHADER_VISIBILITY_VERTEX;
	rootSignatureConf.visibility_srv = D3D12_SHADER_VISIBILITY_PIXEL;
	this->root_signature_ = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void ImGuiManager::createDepthStencil(ID3D12Device* device)
{
	DepthStencil::DepthStencilConf ds_conf = {};
	ds_conf.frame_buffer_count = GraphicsConfigurator::getInstance().getConfigurationData().frame_buffer_count;
	ds_conf.width = GraphicsConfigurator::getInstance().getConfigurationData().window_width;
	ds_conf.height = GraphicsConfigurator::getInstance().getConfigurationData().window_height;
	this->depth_stencil_ = DepthStencilCacheManager::getInstance().getOrCreate(ds_conf, device);
}

void ImGuiManager::createPipelineStateObject(ID3D12Device* device)
{
}
