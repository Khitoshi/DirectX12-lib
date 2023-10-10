#pragma once
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <vector>

#include "Descriptor.h"

using namespace Microsoft::WRL;

class DescriptorHeap;


class DepthStencil :public Descriptor
{
	friend class DepthStencilFactory;
public:
	struct DepthStencilConf
	{
		UINT frame_buffer_count;
		UINT width;
		UINT height;

		bool operator==(const DepthStencilConf& conf) const
		{
			return
				frame_buffer_count == conf.frame_buffer_count &&
				width == conf.width &&
				height == conf.height;
		}
	};

private:
	DepthStencil(const DepthStencilConf c) :
		Descriptor(Descriptor::DescriptorType::DepthStencil),
		conf_(c),
		descriptor_heap_()
	{};

public:
	~DepthStencil() {};

	void init(ID3D12Device* device);

private:
	void createDescriptorHeap(ID3D12Device* device);
	void createResource(ID3D12Device* device);
	void createDSV(ID3D12Device* device);

public:
	ID3D12DescriptorHeap* getDescriptorHeap() const;

private:
	DepthStencilConf conf_;
	std::shared_ptr<DescriptorHeap> descriptor_heap_;

};