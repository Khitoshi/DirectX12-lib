#include <Windows.h>

#include "Window.h"
#include <iostream>

#include "ResourceManager.h"
#include "DX12Resources.h"

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
        WindowConfig winConf;
        winConf.appName = TEXT("DirectX12");
        winConf.x = 0;
        winConf.y = 0;
        winConf.width = 1280;
        winConf.height = 720;
        std::shared_ptr<Window> window = std::make_shared<Window>(winConf, hInstance, nCmdShow);
        window->init();
        ResourceManager::getInstance()->registerResource("window", window);

        //DX12����������
        std::shared_ptr <DX12Resources> dx12Resources = std::make_shared<DX12Resources>();
        dx12Resources->init(ResourceManager::getInstance()->getResource<Window>("window")->getHWND(), winConf.width, winConf.height, 2);
        ResourceManager::getInstance()->registerResource("dx12Resources", dx12Resources);
        ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources");
        //���b�Z�[�W���[�v����
        while (ResourceManager::getInstance()->getResource<Window>("window")->processMessages()) {
            float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
            //�`��J�n����
            ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->beginRender(color);

            //�`��I������
            ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->endRender();
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