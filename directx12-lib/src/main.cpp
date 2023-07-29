#include <Windows.h>

#include "Window.h"
#include <iostream>

#include "ResourceManager.h"
#include "DX12Resources.h"

#include "RootSignature.h"
#include "Triangle.h"
#include "SceneDefault.h"
#include "SceneManager.h"
#include "DeviceContext.h"
#ifdef _DEBUG
#include "imgui/ImGuiManager.h"
#endif // !_DEBUG

//�f�o�b�O�p�o�͊֐�
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    try {
        //windows����������
        Window::WindowConfig winConf = {};
        winConf.app_name = TEXT("DirectX12");
        winConf.x = 0;
        winConf.y = 0;
        std::shared_ptr<Window> window = std::make_shared<Window>(winConf, hInstance, nCmdShow);
        window->init();
        ResourceManager::getInstance()->registerResource("window", window);

        //DX12����������
        std::shared_ptr <DX12Resources> dx12Resources = std::make_shared<DX12Resources>();
        dx12Resources->init(ResourceManager::getInstance()->getResource<Window>("window")->getHWND());
        ResourceManager::getInstance()->registerResource("dx12Resources", dx12Resources);
        dx12Resources = ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources");
        auto device_context = dx12Resources->getDeviceContext();

#ifdef _DEBUG//imgui����������
        ImGuiManager::ImGuiManagerConf imguiConf = {};
        imguiConf.hWnd = ResourceManager::getInstance()->getResource<Window>("window")->getHWND();
        std::shared_ptr<ImGuiManager> imguiManager = std::make_shared<ImGuiManager>(imguiConf);
        imguiManager->init(device_context->getDevice());
        ResourceManager::getInstance()->registerResource("imguiManager", imguiManager);

#endif // _DEBUG

        //�f�o�C�X�擾
        auto device = dx12Resources->getDeviceContext()->getDevice();
        //�����_�[�R���e�L�X�g�擾
        auto rc = dx12Resources->getRenderContext();
        //�V�[���o�^����
        SceneManager::getInstance().registerScene();

        //���b�Z�[�W���[�v����
#ifdef _DEBUG
        //FPS�v���p
        const int FRAMERATE_BUFFER_SIZE = 1000;
        float frameRates[FRAMERATE_BUFFER_SIZE] = {};
        int currentFrameRateIndex = 0;

#endif // _DEBUG
        while (ResourceManager::getInstance()->getResource<Window>("window")->processMessages()) {
            //�`��J�n����
            dx12Resources->beginRender();

#ifdef _DEBUG

            //imguiFrame�J�n����
            ResourceManager::getInstance()->getResource<ImGuiManager>("imguiManager")->beginFrame(rc, device);
            {
                ImGui::Begin("System");

                //�}�E�X���W�\��
                ImVec2 mousePos = ImGui::GetIO().MousePos;
                ImGui::Text("Mouse Position: (%.1f,%.1f)", mousePos.x, mousePos.y);

                //FPS�\��
                frameRates[currentFrameRateIndex] = ImGui::GetIO().Framerate;
                currentFrameRateIndex = (currentFrameRateIndex + 1) % FRAMERATE_BUFFER_SIZE;
                ImGui::Text("FrameRate: %.1f", ImGui::GetIO().Framerate);
                ImGui::PlotLines("FrameRateLine", frameRates, FRAMERATE_BUFFER_SIZE);

                ImGui::End();
            }

            //imgui���j���[�X�V����
            SceneManager::getInstance().updateImguiMenu();
            //�V�[���I��
            SceneManager::getInstance().sceneSelect();
            //Frame�I������
            ResourceManager::getInstance()->getResource<ImGuiManager>("imguiManager")->endFrame();
            //imgui�`�揈��
            ResourceManager::getInstance()->getResource<ImGuiManager>("imguiManager")->render(rc, device);

#endif // _DEBUG

            //�V�[���X�V����
            SceneManager::getInstance().update();
            //�V�[���`�揈��
            SceneManager::getInstance().render(rc);

            //�`��I������
            dx12Resources->endRender();

            //�`��I��������ɃV�[���ύX���s��
            SceneManager::getInstance().changeScene(device);
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

    return EXIT_SUCCESS;
}