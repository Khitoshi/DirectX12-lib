#pragma once
#include "d3dx12.h"
#include <dxgi1_4.h>
using namespace Microsoft::WRL;

/// <summary>
/// デバイスコンテキスト
/// </summary>
class DeviceContext
{
	friend class DeviceContextFactory;
private:
	//GPU情報
	struct GPU_Info {
		std::wstring name_;                //アダプター名
		SIZE_T dedicated_video_memory_;    //VRAM容量
		SIZE_T dedicated_system_memory_;   //VRAM容量
		SIZE_T shared_system_memory_;      //共有メモリ容量

		GPU_Info() :
			name_(),
			dedicated_video_memory_(0),
			dedicated_system_memory_(0),
			shared_system_memory_(0)
		{}

		/// <summary>
		/// descからコピー
		/// </summary>
		/// <param name="desc">アダプタデスク</param>
		void copy(const DXGI_ADAPTER_DESC& desc) {
			this->name_ = desc.Description;
			this->dedicated_video_memory_ = desc.DedicatedVideoMemory / 1024 / 1024;
			this->dedicated_system_memory_ = desc.DedicatedSystemMemory / 1024 / 1024;
			this->shared_system_memory_ = desc.SharedSystemMemory / 1024 / 1024;
		}
	};

private:
	DeviceContext() :
		device_(),
		gpu_info_()
	{};

public:
	~DeviceContext() {};

private:
	//初期化
	void init(IDXGIFactory4* dxgiFactory);

public://取得系
	//デバイスを取得
	ID3D12Device5* getDevice() { return device_.Get(); }

	//GPU情報を取得
	const GPU_Info& getGPUInfo() { return gpu_info_; }

private:
	ComPtr<ID3D12Device5> device_;
	GPU_Info gpu_info_;
};
