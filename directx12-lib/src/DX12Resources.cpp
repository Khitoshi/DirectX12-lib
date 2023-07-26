#include "DX12Resources.h"
#include "OffScreenRenderTargetCacheManager.h"
#include <stdexcept>
void DX12Resources::init(const HWND hWnd, const int width, const int height, const int frameBufferCount)
{
    //���L�����ς��̂ŉ���͍��v��1��ɂȂ�
    ComPtr<IDXGIFactory4> factory = createFactory();
    this->device = createDevice(factory.Get());
    this->commandQueue = createCommandQueue();
    this->swapChain = createSwapChain(hWnd, width, height, frameBufferCount, factory.Get());
    this->commandAllocator = createCommandAllocator();
    this->commandList = createCommandList();
    this->renderTarget = createRenderTarget(width, height, frameBufferCount);
    this->compositeRenderTarget = createCompositeRenderTarget();
    this->depthStencil = createDepthStencil(width, height, frameBufferCount);
    this->fence = createFence();
    this->renderContext = createRenderContext();
    this->viewport = createViewport(width, height);
    this->scissorRect = createScissorRect(width, height);

    fullScreenQuad = std::make_unique<FullScreenQuad>();
    fullScreenQuad->init(this->device.Get());



    //�I�t�X�N���[���Ŏg�p����[�x�X�e���V���r���[�̃n���h����ݒ�
    //OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferRTVHandle);
    OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferDSVHandle);
    //�I�t�X�N���[���Ŏg�p����r���[�|�[�g��ݒ�
    OffScreenRenderTargetCacheManager::getInstance().setViewport(this->viewport);

    //TODO:���t�@�N�^�����O�Ώ�
    OffScreenRenderTarget::OffScreenRenderTargetConf conf = {};
    conf.clearColor[0] = this->backGroundColor[0];
    conf.clearColor[1] = this->backGroundColor[1];
    conf.clearColor[2] = this->backGroundColor[2];
    conf.clearColor[3] = this->backGroundColor[3];
    conf.descriptorHeapDesc = this->renderTarget->getDescriptorHeap()->GetDesc();
    conf.resourceDesc = this->renderTarget->getResource(this->frameIndex)->GetDesc();
    OffScreenRenderTargetCacheManager::getInstance().setConf(conf);
}

/// <summary>
/// �����_�����O�J�n����
/// </summary>
/// <param name="color">clear color</param>
void DX12Resources::beginRender()
{
    //�o�b�N�o�b�t�@�C���f�b�N�X�̎擾
    this->frameIndex = this->swapChain->getCurrentBackBufferIndex();

    //�R�}���h�A���P�[�^�̃��Z�b�g
    this->commandAllocator->Reset();

    //�R�}���h���X�g�̃��Z�b�g
    this->renderContext->reset(this->commandAllocator.Get(), nullptr);

    //�[�x�X�e���V���r���[�̃n���h����ݒ�
    this->setDSVHandle();

    //�I�t�X�N���[���Ŏg�p����[�x�X�e���V���r���[�̃n���h����ݒ�
    //OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferRTVHandle);
    OffScreenRenderTargetCacheManager::getInstance().setDepthStencilViewHandle(this->currentFrameBufferDSVHandle);
    //�I�t�X�N���[���Ŏg�p����r���[�|�[�g��ݒ�
    OffScreenRenderTargetCacheManager::getInstance().setViewport(this->viewport);

    //TODO:���t�@�N�^�����O�Ώ�
    OffScreenRenderTarget::OffScreenRenderTargetConf conf = {};
    conf.clearColor[0] = this->backGroundColor[0];
    conf.clearColor[1] = this->backGroundColor[1];
    conf.clearColor[2] = this->backGroundColor[2];
    conf.clearColor[3] = this->backGroundColor[3];
    conf.descriptorHeapDesc = this->renderTarget->getDescriptorHeap()->GetDesc();
    conf.resourceDesc = this->renderTarget->getResource(this->frameIndex)->GetDesc();
    OffScreenRenderTargetCacheManager::getInstance().setConf(conf);
}

/// <summary>
/// �����_�����O�I������
/// </summary>
void DX12Resources::endRender()
{
    //offscreen��1���̃e�N�X�`���ɂ܂Ƃ߂�`��J�n����
    this->compositeRenderTarget->beginRender(this->renderContext.get(), this->currentFrameBufferDSVHandle);
    //offscreen��1���̃e�N�X�`���ɂ܂Ƃ߂�`�揈��
    this->compositeRenderTarget->render(this->renderContext.get(), this->device.Get());
    //offscreen��1���̃e�N�X�`���ɂ܂Ƃ߂�`��I������
    this->compositeRenderTarget->endRender(this->renderContext.get());

    //Main�����_�[�^�[�Q�b�g�̕`��J�n

    //�����_�[�^�[�Q�b�g�̐ݒ�
    this->setMainRTVHandle();

    //�r���[�|�[�g�ƃV�U�����O��`�̐ݒ�
    this->renderContext->transitionMainRenderTargetBegin(this->renderTarget->getResource(this->frameIndex));
    this->renderContext->executeResourceBarriers();
    this->renderContext->setRenderTarget(this->currentFrameBufferRTVHandle, this->currentFrameBufferDSVHandle);
    this->renderContext->setViewport(this->viewport);
    this->renderContext->setScissorRect(this->viewport);

    //�[�x�X�e���V���̃N���A
    this->renderContext->clearRenderTarget(this->currentFrameBufferRTVHandle, this->backGroundColor);
    this->renderContext->clearDepthStencil(this->currentFrameBufferDSVHandle, 1.0f);

    //offscreen���e�N�X�`���Ƃ����t���X�N���[���l�p�`��`��
    fullScreenQuad->draw(this->renderContext.get(), this->device.Get(), compositeRenderTarget.get());

    //Main�����_�[�^�[�Q�b�g�̕`�抮����҂�
    this->renderContext->transitionMainRenderTargetEnd(this->renderTarget->getResource(this->frameIndex));

    //���\�[�X�o���A�̎��s
    this->renderContext->executeResourceBarriers();

    //�R�}���h�̃N���[�Y
    this->renderContext->close();

    //�R�}���h�̎��s
    ID3D12CommandList* cmd_list[] = { this->commandList.Get() };
    this->commandQueue->ExecuteCommandLists(_countof(cmd_list), cmd_list);

    //�X���b�v�`�F�C���̍X�V
    this->swapChain->present();

    //�`�抮����҂�
    waitEndOfDrawing();
}

/// <summary>
/// �`��̏I����҂�
/// </summary>
void DX12Resources::waitEndOfDrawing()
{
    //�t�F���X�̃V�O�i��
    this->commandQueue->Signal(this->fence->getFence(), this->fence->getValue());

    //�t�F���X�̑ҋ@
    if (this->fence->getFence()->GetCompletedValue() < this->fence->getValue()) {
        this->fence->getFence()->SetEventOnCompletion(this->fence->getValue(), this->fence->getEvent());
        WaitForSingleObject(this->fence->getEvent(), INFINITE);
    }

    //�t�F���X�̒l���X�V
    this->fence->incrementValue();
}

/// <summary>
/// �t�@�N�g���@����
/// </summary>
/// <returns>
/// ���������t�@�N�g���̃|�C���g
/// </returns>
ComPtr<IDXGIFactory4> DX12Resources::createFactory() {
    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    //�f�o�b�O�R���g���[���[������΁A�f�o�b�O���C���[������DXGI���쐬����B
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();

        //�f�o�b�O�̏ꍇ�f�o�b�O�t���O�𗧂Ă�
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#if 0 // GBV ��L��������ꍇ.
        ComPtr<ID3D12Debug3> debug3;
        debug.As(&debug3);
        if (debug3)
        {
            debug3->SetEnableGPUBasedValidation(true);
        }
#endif
    }
    else {
        throw std::runtime_error("failed to create debug Controller");
    }

#endif

    ComPtr<IDXGIFactory4> factory;

    //factory����
    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("failed to create dxgi factory");
    }

    return factory;
}

/// <summary>
/// �f�o�C�X����
/// </summary>
/// <param name="dxgiFactory">IDXGIfactory</param>
/// <returns>
///	���������f�o�C�X�̃|�C���^
/// </returns>
ComPtr<ID3D12Device5> DX12Resources::createDevice(IDXGIFactory4* dxgiFactory)
{
    //GPU�x���_�[��`
    enum class GPU_Vender {
        NVIDIA,	//NVIDIA
        AMD,	//Intel
        INTEL,	//AMD

        NUM,	//Vender��
    };

    IDXGIAdapter* adapterTmp = nullptr;
    IDXGIAdapter* adapterVender[(int)GPU_Vender::NUM] = { nullptr };//�e�x���_�[�̃A�_�v�^�[
    IDXGIAdapter* adapterMaxVideoMemory = nullptr;					//�ő�r�f�I�������̃A�_�v�^
    IDXGIAdapter* useAdapter = nullptr;								//�ŏI�I�Ɏg�p����A�_�v�^
    SIZE_T videoMemorySize = 0;										//�ő�r�f�I�������̃T�C�Y

    for (int i = 0; dxgiFactory->EnumAdapters(i, &adapterTmp) != DXGI_ERROR_NOT_FOUND; i++) {
        //�A�_�v�^�[�̏����擾�B
        DXGI_ADAPTER_DESC desc;
        adapterTmp->GetDesc(&desc);

        //�ő�r�f�I�������̃A�_�v�^�[��ۑ��B
        //�����Ŏ擾�����A�_�v�^��AMD��INTEL����GPU���Ȃ��ꍇ�Ɏg�p����A�_�v�^
        if (desc.DedicatedVideoMemory > videoMemorySize) {
            //���łɓo�^���Ă���ꍇ�́A������Ă���o�^�B
            if (adapterMaxVideoMemory != nullptr) {
                adapterMaxVideoMemory->Release();
            }
            adapterMaxVideoMemory = adapterTmp;
            adapterMaxVideoMemory->AddRef();
            videoMemorySize = desc.DedicatedVideoMemory;
        }

        //�x���_�[�ʂɃA�_�v�^�[��ۑ��B
        //���łɓo�^���Ă���ꍇ�́A������Ă���o�^
        if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {//NVIDIA
            if (adapterVender[(int)GPU_Vender::NVIDIA]) {
                adapterVender[(int)GPU_Vender::NVIDIA]->Release();
            }
            adapterVender[(int)GPU_Vender::NVIDIA] = adapterTmp;
            adapterVender[(int)GPU_Vender::NVIDIA]->AddRef();
        }
        else if (wcsstr(desc.Description, L"AMD") != nullptr) {//AMD
            if (adapterVender[(int)GPU_Vender::AMD]) {
                adapterVender[(int)GPU_Vender::AMD]->Release();
            }
            adapterVender[(int)GPU_Vender::AMD] = adapterTmp;
            adapterVender[(int)GPU_Vender::AMD]->AddRef();
        }
        else if (wcsstr(desc.Description, L"Intel") != nullptr) {//Intel
            if (adapterVender[(int)GPU_Vender::INTEL]) {
                adapterVender[(int)GPU_Vender::INTEL]->Release();
            }
            adapterVender[(int)GPU_Vender::INTEL] = adapterTmp;
            adapterVender[(int)GPU_Vender::INTEL]->AddRef();
        }

        //�A�_�v�^�[�����
        adapterTmp->Release();
    }

    //�g�p����A�_�v�^�����߂�(���݂�NVIDIA���ŗD��)
    // NVIDIA >> AMD >> intel >> other
    if (adapterVender[(int)GPU_Vender::NVIDIA] != nullptr) useAdapter = adapterVender[(int)GPU_Vender::NVIDIA];
    else if (adapterVender[(int)GPU_Vender::AMD] != nullptr) useAdapter = adapterVender[(int)GPU_Vender::AMD];
    else if (adapterVender[(int)GPU_Vender::INTEL] != nullptr) useAdapter = adapterVender[(int)GPU_Vender::INTEL];
    else useAdapter = adapterMaxVideoMemory;

    //�g�p���Ă���\���̂���FEATURE_LEVEL���
    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    ComPtr<ID3D12Device5> device;
    //D3D_FEATURE_LEVEL�̃��x���̍�������loop
    //HACK:https://forums.developer.nvidia.com/t/poco-notfoundexception-thrown-in-driver-version-531-18/245285
    //Poco::NotFoundException�̗�O�X���[���������C������NVIDIA�̃h���C�o
    for (auto featureLevel : featureLevels) {
        if (SUCCEEDED(D3D12CreateDevice(
            useAdapter,
            featureLevel,
            IID_PPV_ARGS(&device)
        ))) {//�����ɐ���
            break;
        }
    }

    //�擾�����S�ẴO���t�B�b�N�J�[�h���������
    for (auto& adapter : adapterVender) {
        if (adapter) {//���݂���ꍇ���
            adapter->Release();
        }
    }

    //GPU�ȊO�ŃO���t�B�b�N�\�͂̂���f�o�C�X���
    if (adapterMaxVideoMemory) {
        adapterMaxVideoMemory->Release();
    }

    //�f�o�C�X�������ł��Ȃ������ꍇ�G���[
    if (device == nullptr) {
        throw std::runtime_error("failed to create device");
    }

    return device;
}

/// <summary>
/// �R�}���h�L���[�̐���
/// </summary>
/// <returns>���������R�}���h�L���[�̃|�C���^</returns>
ComPtr<ID3D12CommandQueue> DX12Resources::createCommandQueue()
{
    //�R�}���h�L���[�̐ݒ�
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.NodeMask = 0;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    //�R�}���h�L���[����
    ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
    if (FAILED(this->device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)))) {
        throw std::runtime_error("failed to create command queue");
    }

    return commandQueue;
}

/// <summary>
/// �X���b�v�`�F�C������
/// </summary>
/// <param name="hWnd">�E�B���h�E�n���h��</param>
/// <param name="width">��</param>
/// <param name="height">����</param>
/// <param name="factory">dxgi�t�@�N�g��</param>
/// <returns>
/// ���������X���b�v�`�F�C��
/// </returns>
std::shared_ptr<SwapChain> DX12Resources::createSwapChain(const HWND hWnd, const int width, const int height, const int frameBufferCount, IDXGIFactory4* factory) {
    std::shared_ptr<SwapChain> swapChain(new SwapChain());

    SwapChainConf conf = {};
    conf.hWnd = hWnd;
    conf.width = width;
    conf.height = height;
    conf.frameBufferCount = frameBufferCount;
    conf.factory = factory;
    conf.commandQueue = this->commandQueue.Get();
    swapChain->init(conf);

    return swapChain;
}

/// <summary>
/// �R�}���h�A���P�[�^����
/// </summary>
/// <returns>
/// ���������R�}���h�A���P�[�^
/// </returns>
ComPtr<ID3D12CommandAllocator> DX12Resources::createCommandAllocator()
{
    //�R�}���h�A���P�[�^����
    ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
    if (FAILED(this->device.Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) {
        throw std::runtime_error("failed to create command allocator");
    }

    return commandAllocator;
}

/// <summary>
/// �R�}���h���X�g����
/// </summary>
/// <returns>
/// ���������R�}���h���X�g
/// </returns>
ComPtr<ID3D12GraphicsCommandList4> DX12Resources::createCommandList()
{
    ComPtr<ID3D12GraphicsCommandList4> commandList = nullptr;
    //TODO:���݂�GPU��1�Ȃ̂�nodemask��0�ɂ��Ă��邩��C����GPU�ɑΉ�����ꍇ�͕ύX����K�v������
    //�R�}���h���X�g����
    if (FAILED(this->device.Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) {
        throw std::runtime_error("failed to create command list");
    }

    //�R�}���h���X�g�������ɂ͂܂��R�}���h���X�g�̓N���[�Y����Ă��Ȃ��̂ŁC�N���[�Y����
    if (FAILED(commandList->Close())) {
        throw std::runtime_error("failed to close command list");
    }

    return commandList;
}

/// <summary>
/// �����_�[�^�[�Q�b�g����
/// </summary>
/// <param name="width">��</param>
/// <param name="height">����</param>
/// <param name="frameBufferCount">�t���[���o�b�t�@��</param>
/// <returns>
/// ����&���������������_�[�^�[�Q�b�g
/// </returns>
std::shared_ptr<RenderTarget> DX12Resources::createRenderTarget(const int width, const int height, const UINT frameBufferCount)
{
    RenderTargetConf rtc = {};
    rtc.device = this->device.Get();
    rtc.frameBufferCount = frameBufferCount;
    rtc.width = width;
    rtc.height = height;
    rtc.swapChain = this->swapChain->getSwapChain();

    std::shared_ptr<RenderTarget> renderTarget(new RenderTarget());
    renderTarget->init(rtc);
    return renderTarget;
}

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g����
/// </summary>
/// <returns>
/// ����&�����������I�t�X�N���[�������_�[�^�[�Q�b�g
/// </returns>
std::shared_ptr<CompositeRenderTarget> DX12Resources::createCompositeRenderTarget()
{
    CompositeRenderTarget::CompositeRenderTargetConf conf = {};
    conf.resource_desc = this->renderTarget->getResource(0)->GetDesc();
    conf.descriptor_heap_desc = this->renderTarget->getDescriptorHeap()->GetDesc();
    conf.clear_color[0] = this->backGroundColor[0];
    conf.clear_color[1] = this->backGroundColor[1];
    conf.clear_color[2] = this->backGroundColor[2];
    conf.clear_color[3] = this->backGroundColor[3];
    std::shared_ptr<CompositeRenderTarget> crt = {};
    crt = std::make_shared<CompositeRenderTarget>(conf);
    crt->init(this->device.Get());
    return crt;
}

/// <summary>
/// �[�x�X�e���V������
/// </summary>
/// <param name="width">��</param>
/// <param name="height">����</param>
/// <param name="frameBufferCount">�o�b�t�@�ԍ�</param>
/// <returns>
/// ����&�����������[�x�X�e���V��
/// </returns>
std::shared_ptr<DepthStencil> DX12Resources::createDepthStencil(const int width, const int height, const UINT frameBufferCount)
{
    DepthStencil::DepthStencilConf ds_conf = {};
    ds_conf.width = width;
    ds_conf.height = height;
    ds_conf.frame_buffer_count = frameBufferCount;

    std::shared_ptr<DepthStencil> depthStencil = std::make_shared<DepthStencil>(ds_conf);
    depthStencil->init(this->device.Get());
    return depthStencil;
}

/// <summary>
/// GPU��CPU�̓����I�u�W�F�N�g(fence)����
/// </summary>
/// <returns>
/// ����&����������fence
/// </returns>
std::shared_ptr<Fence> DX12Resources::createFence()
{
    std::shared_ptr<Fence>fence = std::make_shared<Fence>();
    fence->init(device.Get());
    return fence;
}

/// <summary>
/// �����_�[�R���e�L�X�g�̐���
/// </summary>
/// <returns>
/// ����&���������������_�[�R���e�L�X�g
/// </returns>
std::shared_ptr<RenderContext> DX12Resources::createRenderContext()
{
    std::shared_ptr<RenderContext> rc = std::make_shared<RenderContext>();
    rc->init(this->commandList.Get());
    return rc;
}

/// <summary>
/// �r���[�|�[�g��ݒ肷��
/// </summary>
/// <param name="width">��</param>
/// <param name="height">����</param>
/// <returns>
/// �ݒ��̃r���[�|�[�g
/// </returns>
D3D12_VIEWPORT DX12Resources::createViewport(const int width, const int height)
{
    D3D12_VIEWPORT vp = {};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = D3D12_MIN_DEPTH;
    vp.MaxDepth = D3D12_MAX_DEPTH;
    return vp;
}

/// <summary>
/// �V�U�����O��`��ݒ肷��
/// </summary>
/// <param name="width">��</param>
/// <param name="height">����</param>
/// <returns>
/// �ݒ�ς݂̃V�U�����O��`
/// </returns>
D3D12_RECT DX12Resources::createScissorRect(const int width, const int height)
{
    D3D12_RECT rect = {};
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    return rect;
}

/// <summary>
/// RTV�n���h����ݒ肷��
/// </summary>
void DX12Resources::setMainRTVHandle()
{
    //�����_�[�^�[�Q�b�g�̃f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
    this->currentFrameBufferRTVHandle = this->renderTarget->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    //�t���[���o�b�t�@�������炷
    this->currentFrameBufferRTVHandle.ptr += static_cast<unsigned long long>(this->renderTarget->getDescriptorHeapSize()) * this->frameIndex;
}

void DX12Resources::setOffScreenRTVHandle()
{
    this->currentFrameBufferRTVHandle = this->compositeRenderTarget->getRTVHeap()->GetCPUDescriptorHandleForHeapStart();
    //this->currentFrameBufferRTVHandle.ptr += static_cast<unsigned long long>(this->offScreenRenderTarget->getDescriptorHeapSize()) * this->frameIndex;
}

/// <summary>
/// DSV�n���h����ݒ肷��
/// </summary>
void DX12Resources::setDSVHandle()
{
    this->currentFrameBufferDSVHandle = this->depthStencil->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
}

