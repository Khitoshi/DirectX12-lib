#include "DeviceContext.h"
#include <stdexcept>

/// <summary>
/// �f�o�C�X�̏�����
/// </summary>
/// <param name="dxgiFactory">dxgiFactory</param>
void DeviceContext::init(IDXGIFactory4* dxgiFactory)
{
    //GPU�x���_�[��`
    enum class GPU_VENDER {
        NVIDIA,	//NVIDIA
        AMD,	//Intel
        INTEL,	//AMD

        NUM,	//Vender��
    };

    IDXGIAdapter* adapter_tmp = nullptr;
    IDXGIAdapter* adapter_vender[(int)GPU_VENDER::NUM] = { nullptr };//�e�x���_�[�̃A�_�v�^�[
    IDXGIAdapter* adapter_max_video_memory = nullptr;					//�ő�r�f�I�������̃A�_�v�^
    IDXGIAdapter* use_adapter = nullptr;								//�ŏI�I�Ɏg�p����A�_�v�^
    SIZE_T video_memory_size = 0;										//�ő�r�f�I�������̃T�C�Y

    for (int i = 0; dxgiFactory->EnumAdapters(i, &adapter_tmp) != DXGI_ERROR_NOT_FOUND; i++) {
        //�A�_�v�^�[�̏����擾�B
        DXGI_ADAPTER_DESC desc;
        adapter_tmp->GetDesc(&desc);

        //�ő�r�f�I�������̃A�_�v�^�[��ۑ��B
        //�����Ŏ擾�����A�_�v�^��AMD��INTEL����GPU���Ȃ��ꍇ�Ɏg�p����A�_�v�^
        if (desc.DedicatedVideoMemory > video_memory_size) {
            //���łɓo�^���Ă���ꍇ�́A������Ă���o�^�B
            if (adapter_max_video_memory != nullptr) {
                adapter_max_video_memory->Release();
            }
            adapter_max_video_memory = adapter_tmp;
            adapter_max_video_memory->AddRef();
            video_memory_size = desc.DedicatedVideoMemory;
        }

        //�x���_�[�ʂɃA�_�v�^�[��ۑ��B
        //���łɓo�^���Ă���ꍇ�́A������Ă���o�^
        if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {//NVIDIA
            if (adapter_vender[(int)GPU_VENDER::NVIDIA]) {
                adapter_vender[(int)GPU_VENDER::NVIDIA]->Release();
            }
            adapter_vender[(int)GPU_VENDER::NVIDIA] = adapter_tmp;
            adapter_vender[(int)GPU_VENDER::NVIDIA]->AddRef();
        }
        else if (wcsstr(desc.Description, L"AMD") != nullptr) {//AMD
            if (adapter_vender[(int)GPU_VENDER::AMD]) {
                adapter_vender[(int)GPU_VENDER::AMD]->Release();
            }
            adapter_vender[(int)GPU_VENDER::AMD] = adapter_tmp;
            adapter_vender[(int)GPU_VENDER::AMD]->AddRef();
        }
        else if (wcsstr(desc.Description, L"Intel") != nullptr) {//Intel
            if (adapter_vender[(int)GPU_VENDER::INTEL]) {
                adapter_vender[(int)GPU_VENDER::INTEL]->Release();
            }
            adapter_vender[(int)GPU_VENDER::INTEL] = adapter_tmp;
            adapter_vender[(int)GPU_VENDER::INTEL]->AddRef();
        }
        //�A�_�v�^�[�����
        adapter_tmp->Release();
    }

    //�g�p����A�_�v�^�����߂�(���݂�NVIDIA���ŗD��)
    // NVIDIA >> AMD >> intel >> other
    if (adapter_vender[(int)GPU_VENDER::NVIDIA] != nullptr)use_adapter = adapter_vender[(int)GPU_VENDER::NVIDIA];
    else if (adapter_vender[(int)GPU_VENDER::AMD] != nullptr) use_adapter = adapter_vender[(int)GPU_VENDER::AMD];
    else if (adapter_vender[(int)GPU_VENDER::INTEL] != nullptr)use_adapter = adapter_vender[(int)GPU_VENDER::INTEL];
    else use_adapter = adapter_max_video_memory;
    DXGI_ADAPTER_DESC desc;
    use_adapter->GetDesc(&desc);
    this->gpu_info_.copy(desc);

    //�g�p���Ă���\���̂���FEATURE_LEVEL���
    const D3D_FEATURE_LEVEL FEATURELEVELS[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    //D3D_FEATURE_LEVEL�̃��x���̍�������loop
    //HACK:https://forums.developer.nvidia.com/t/poco-notfoundexception-thrown-in-driver-version-531-18/245285
    //Poco::NotFoundException�̗�O�X���[���������C������NVIDIA�̃h���C�o
    for (auto feature_level : FEATURELEVELS) {
        if (SUCCEEDED(D3D12CreateDevice(
            use_adapter,
            feature_level,
            IID_PPV_ARGS(&this->device_)
        ))) {//�����ɐ���
            break;
        }
    }

    //�擾�����S�ẴO���t�B�b�N�J�[�h���������
    for (auto& adapter : adapter_vender) {
        if (adapter) {//���݂���ꍇ���
            adapter->Release();
        }
    }
    //GPU�ȊO�ŃO���t�B�b�N�\�͂̂���f�o�C�X���
    if (adapter_max_video_memory) {
        adapter_max_video_memory->Release();
    }

    //�f�o�C�X�������ł��Ȃ������ꍇ�G���[
    if (this->device_ == nullptr) {
        throw std::runtime_error("failed to create device");
    }
}