#include "Framework.h"
#include "SceneManager.h"
#include "DX12Resources.h"
#include "imgui/ImGuiManager.h"

#include "DeviceContext.h"

#include <exception>
#include <iostream>



/// <summary>
/// �f�o�b�O�p�o�͊֐�
/// </summary>
/// <param name="format"></param>
/// <param name=""></param>
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
    va_list valist;
    va_start(valist, format);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), format, valist);
    OutputDebugStringA(buf);
    va_end(valist);
#endif
}

/// <summary>
/// ���s����
/// </summary>
/// <param name="processMessages">���b�Z�[�W�������[�v</param>
/// <returns>
/// ����I��: EXIT_SUCCESS��Ԃ�
/// �G���[ : EXIT_FAILURE��Ԃ�
/// </returns>
int Framework::run(std::function<bool()> processMessages)
{
    MSG msg = {};
    try {
        //�A�v���P�[�V�������I��鎞��message��WM_QUIT�ɂȂ�
        while (processMessages()) {
            this->update();
            this->render();
        }
    }
    catch (const std::exception& e) {
        //�G���[���R���\�[���ɕ\��
        DebugOutputFormatString("[!!!ERROR!!!] Caught an exception: %s", e.what());
        return EXIT_FAILURE;
    }
    catch (...) {
        //�G���[���R���\�[���ɕ\��
        DebugOutputFormatString("[!!!ERROR!!!] Caught an unknown exception.");
        return EXIT_FAILURE;
    }
    this->deinit();

    return EXIT_SUCCESS;
}

/// <summary>
/// �X�V����
/// </summary>
void Framework::init()
{
    //DX12����������
    this->dx12_resources_ = std::make_shared<DX12Resources>();
    this->dx12_resources_->init(this->hWnd_);

    //imgui����������
    this->imgui_manager_ = std::make_shared<ImGuiManager>();
    this->imgui_manager_->init(this->dx12_resources_->getDeviceContext()->getDevice(), this->hWnd_);

    //�V�[���o�^����
    //SceneManager::getInstance().registerScene();
    SceneManager::getInstance().init(this->dx12_resources_->getDeviceContext()->getDevice(), std::make_shared<SceneTriangle>());
}

/// <summary>
/// �X�V����
/// </summary>
void Framework::update()
{

    this->dx12_resources_->beginRender();

    SceneManager::getInstance().update();
}

/// <summary>
/// ���C���`�揈��
/// </summary>
void Framework::render()
{
    debugRender();
    SceneManager::getInstance().render(this->dx12_resources_->getRenderContext());
    this->dx12_resources_->endRender();

    auto device_context = this->dx12_resources_->getDeviceContext();
    SceneManager::getInstance().changeScene(device_context->getDevice());
}

/// <summary>
/// �f�o�b�O�p�`�揈��
/// </summary>
void Framework::debugRender()
{
    //imguiFrame�J�n����
    auto render_context = this->dx12_resources_->getRenderContext();
    auto device_context = this->dx12_resources_->getDeviceContext();

    this->imgui_manager_->beginFrame(render_context, device_context->getDevice());
    ImGui::Begin("System");

    //�}�E�X���W�\��
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImGui::Text("Mouse Position: (%.1f,%.1f)", mousePos.x, mousePos.y);

    //FPS�\��
    //frameRates[currentFrameRateIndex] = ImGui::GetIO().Framerate;
    //currentFrameRateIndex = (currentFrameRateIndex + 1) % FRAMERATE_BUFFER_SIZE;
    ImGui::Text("FrameRate: %.1f", ImGui::GetIO().Framerate);
    //ImGui::PlotLines("FrameRateLine", frameRates, FRAMERATE_BUFFER_SIZE);

    auto gpu_info = device_context->getGPUInfo();
    ImGui::Text("Adaptor : %ls", gpu_info.name_.c_str());
    ImGui::Text("Dedicated Video Memory: %d", gpu_info.dedicated_video_memory_);
    ImGui::Text("Dedicated System Memory: %d", gpu_info.dedicated_system_memory_);
    ImGui::Text("Shared System Memory: %d", gpu_info.shared_system_memory_);

    ImGui::End();

    //imgui���j���[�X�V����
    SceneManager::getInstance().updateImguiMenu();
    //�V�[���I��
    SceneManager::getInstance().sceneSelect();

    //Frame�I������
    this->imgui_manager_->endFrame();
    //imgui�`�揈��
    this->imgui_manager_->render(render_context, device_context->getDevice());

}

/// <summary>
/// �I������
/// </summary>
void Framework::deinit()
{
    this->imgui_manager_->deinit();
    this->dx12_resources_->waitEndOfDrawing();
}