#pragma once
#include "d3dx12.h"
#include <dxgi1_4.h>
using namespace Microsoft::WRL;

/// <summary>
/// �f�o�C�X�R���e�L�X�g���b�s���O�N���X
/// </summary>
class DeviceContext
{
	friend class DeviceContextFactory;
private:
	//GPU���
	struct GPU_Info {
		std::wstring name_;                //�A�_�v�^�[��
		SIZE_T dedicated_video_memory_;    //VRAM�e��
		SIZE_T dedicated_system_memory_;   //VRAM�e��
		SIZE_T shared_system_memory_;      //���L�������e��

		GPU_Info() :
			name_(),
			dedicated_video_memory_(0),
			dedicated_system_memory_(0),
			shared_system_memory_(0)
		{}

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

private://�������n

	void init(IDXGIFactory4* dxgiFactory);

public://�擾�n
	ID3D12Device5* getDevice() { return device_.Get(); }
	const GPU_Info& getGPUInfo() { return gpu_info_; }

private:
	ComPtr<ID3D12Device5> device_;
	GPU_Info gpu_info_;
};
