#pragma once
#include "d3dx12.h"
#include <string>
#include <chrono>
#include <dxgi1_4.h>
#include <memory>
#include "Cache.h"
using namespace Microsoft::WRL;

class Descriptor
{
public:
	enum class DescriptorType {
		Vertex,
		Index,
		Constant,
		ShaderResource,
		UnorderedAccess,
		MainRenderTarget,
		OffScreenRenderTarget,
		CompositeRenderTarget,
		DepthStencil,
		Sampler,

		Num
	};

protected:
	Descriptor(DescriptorType type) :
		resource_()
	{
		Descriptor::cache_->addDescriptor(type, this);
	}

public:
	~Descriptor() {}

protected:
	void createCommittedResource(
		ID3D12Device* device,
		const D3D12_HEAP_PROPERTIES& heap,
		const D3D12_HEAP_FLAGS& flag,
		const D3D12_RESOURCE_DESC& desc,
		const D3D12_RESOURCE_STATES& status,
		const D3D12_CLEAR_VALUE* clear_value
	);

	void setName(const LPCWSTR& name);

public:
	template<typename T>
	void map(T* src, const UINT array_length)
	{
		void* pData = nullptr;
		if (FAILED(this->resource_->Map(0, nullptr, (void**)&pData))) {
			throw std::runtime_error("FAILED Descriptor::map Map");
		}

		const UINT size = sizeof(T) * array_length;

		memcpy(pData, src, size);

		this->resource_->Unmap(0, nullptr);
	}

	//再生成関数
	virtual void init(ID3D12Device* device) = 0;

	//リソースのリリース
	void release() {
		this->resource_.Reset();
	}

public:
	D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress()
	{
		return this->resource_->GetGPUVirtualAddress();
	}

	ID3D12Resource* getResource()const
	{
		return this->resource_.Get();
	}

	ID3D12Resource** getResourceAddress()
	{
		return this->resource_.GetAddressOf();
	}

	const static std::shared_ptr<Cache<DescriptorType, Descriptor>> getCache()
	{
		return Descriptor::cache_;
	}

private:
	ComPtr<ID3D12Resource> resource_;

	static std::shared_ptr<Cache<DescriptorType, Descriptor>> cache_;

};

inline std::shared_ptr<Cache<Descriptor::DescriptorType, Descriptor>> Descriptor::cache_ = std::make_shared<Cache<DescriptorType, Descriptor>>();;