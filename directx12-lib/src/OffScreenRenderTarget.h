#pragma once

#include "d3dx12.h"
#include "Descriptor.h"

class RenderContext;
class DepthStencil;
class DescriptorHeap;

class OffScreenRenderTarget :public Descriptor
{
	friend class OffScreenRenderTargetFactory;
public:
	struct OffScreenRenderTargetConf {
		D3D12_RESOURCE_DESC resource_desc; 		            //バックバッファの設定
		D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;    //バックバッファで使用しているディスクリプタヒープの設定

		bool operator==(const OffScreenRenderTargetConf& conf) const {
			return
				resource_desc.Width == conf.resource_desc.Width &&
				resource_desc.Height == conf.resource_desc.Height &&
				resource_desc.Format == conf.resource_desc.Format &&
				resource_desc.DepthOrArraySize == conf.resource_desc.DepthOrArraySize &&
				resource_desc.MipLevels == conf.resource_desc.MipLevels &&
				resource_desc.SampleDesc.Count == conf.resource_desc.SampleDesc.Count &&
				resource_desc.SampleDesc.Quality == conf.resource_desc.SampleDesc.Quality &&
				resource_desc.Dimension == conf.resource_desc.Dimension &&
				resource_desc.Layout == conf.resource_desc.Layout &&
				resource_desc.Flags == conf.resource_desc.Flags &&

				descriptor_heap_desc.NumDescriptors == conf.descriptor_heap_desc.NumDescriptors &&
				descriptor_heap_desc.Type == conf.descriptor_heap_desc.Type;
		}
	};

private:
	OffScreenRenderTarget(OffScreenRenderTargetConf c) :
		Descriptor(Descriptor::DescriptorType::OffScreenRenderTarget),
		conf_(c),
		cbv_srv_uav_descriptor_heap_(),
		rtv_descriptor_heap_(),
		viewport_(),
		device_()
	{};

public:
	~OffScreenRenderTarget() {};

	void beginRender(RenderContext* rc, D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle);
	void endRender(RenderContext* rc);
	void createRenderTargetView(ID3D12Device* device);

private:
	void init(ID3D12Device* device);

	void createResource(ID3D12Device* device);
	void createSRVHeap(ID3D12Device* device);
	void createShaderResourceView(ID3D12Device* device);
	void createRTVHeap(ID3D12Device* device);

public://設定系
	void setViewport(const D3D12_VIEWPORT vp) { this->viewport_ = vp; }

public://取得系
	ID3D12DescriptorHeap* getSRVHeap()  const;
	ID3D12DescriptorHeap* getRTVHeap()  const;

private:
	OffScreenRenderTargetConf conf_;

	std::shared_ptr<DescriptorHeap> cbv_srv_uav_descriptor_heap_;
	std::shared_ptr<DescriptorHeap> rtv_descriptor_heap_;
	D3D12_VIEWPORT viewport_;

	ID3D12Device* device_;
};
