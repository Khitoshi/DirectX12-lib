#include <Windows.h>

#include "Window.h"
#include <iostream>

#include "ResourceManager.h"
#include "DX12Resources.h"

#include "RootSignature.h"
#include "Triangle.h"
#include "SceneDefault.h"
#include "SceneManager.h"

#ifdef _DEBUG
#include "imgui\ImGuiManager.h"
#endif // !_DEBUG

//デバッグ用出力関数
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

#define FRAMEBUFFERCOUNT 2

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    try {
        //windows初期化処理
        WindowConfig winConf = {};
        winConf.appName = TEXT("DirectX12");
        winConf.x = 0;
        winConf.y = 0;
        winConf.width = 1280;
        winConf.height = 720;
        std::shared_ptr<Window> window = std::make_shared<Window>(winConf, hInstance, nCmdShow);
        window->init();
        ResourceManager::getInstance()->registerResource("window", window);

        //DX12初期化処理
        std::shared_ptr <DX12Resources> dx12Resources = std::make_shared<DX12Resources>();
        dx12Resources->init(ResourceManager::getInstance()->getResource<Window>("window")->getHWND(), winConf.width, winConf.height, FRAMEBUFFERCOUNT);
        ResourceManager::getInstance()->registerResource("dx12Resources", dx12Resources);

#ifdef _DEBUG//imgui初期化処理
        ImGuiManagerConf imguiConf = {};
        imguiConf.device = ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->getDevice();
        imguiConf.frameBufferCount = FRAMEBUFFERCOUNT;
        imguiConf.hWnd = ResourceManager::getInstance()->getResource<Window>("window")->getHWND();
        std::shared_ptr<ImGuiManager> imguiManager = std::make_shared<ImGuiManager>();
        imguiManager->init(imguiConf);
        ResourceManager::getInstance()->registerResource("imguiManager", imguiManager);
#endif // _DEBUG

        //デバイス取得
        auto device = ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->getDevice();
        //レンダーコンテキスト取得
        auto rc = ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->getRenderContext();

        //シーン共通の描画設定
        SceneConf sceneConf = {};
        sceneConf.device = device;
        sceneConf.renderContext = rc;
        //シーン登録処理
        SceneManager::getInstance().registerScene();

        //メッセージループ処理
        //float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
#ifdef _DEBUG
        //FPS計測用
        const int FRAMERATE_BUFFER_SIZE = 1000;
        float frameRates[FRAMERATE_BUFFER_SIZE] = {};
        int currentFrameRateIndex = 0;

#endif // _DEBUG
        while (ResourceManager::getInstance()->getResource<Window>("window")->processMessages()) {
            //描画開始処理
            ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->setBackGroundColor(color);
            ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->beginRender();

            SceneManager::getInstance().update();
            SceneManager::getInstance().render(sceneConf);

#ifdef _DEBUG
            //imguiFrame開始処理
            ResourceManager::getInstance()->getResource<ImGuiManager>("imguiManager")->beginFrame();

            {
                ImGui::Begin("System");

                //背景色変更
                ImGui::ColorEdit4("back color", color);

                //マウス座標表示
                ImVec2 mousePos = ImGui::GetIO().MousePos;
                ImGui::Text("Mouse Position: (%.1f,%.1f)", mousePos.x, mousePos.y);

                //FPS表示
                frameRates[currentFrameRateIndex] = ImGui::GetIO().Framerate;
                currentFrameRateIndex = (currentFrameRateIndex + 1) % FRAMERATE_BUFFER_SIZE;
                ImGui::Text("FrameRate: %.1f", ImGui::GetIO().Framerate);
                ImGui::PlotLines("FrameRateLine", frameRates, FRAMERATE_BUFFER_SIZE);

                ImGui::End();
            }

            //imguiメニュー更新処理
            SceneManager::getInstance().updateImguiMenu();
            //シーン選択
            SceneManager::getInstance().sceneSelect();

            //Frame終了処理
            ResourceManager::getInstance()->getResource<ImGuiManager>("imguiManager")->endFrame();
            //imgui描画処理
            ResourceManager::getInstance()->getResource<ImGuiManager>("imguiManager")->render(ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->getRenderContext());
#endif // _DEBUG

            //描画終了処理
            ResourceManager::getInstance()->getResource<DX12Resources>("dx12Resources")->endRender();

            //描画終了処理後にシーン変更を行う
            SceneManager::getInstance().changeScene(sceneConf);
        }
    }
    catch (const std::exception& e) {
        //エラーをコンソールに表示
        DebugOutputFormatString("[!!!ERROR!!!] Caught an exception: %s", e.what());
        return EXIT_FAILURE;
    }
    catch (...) {
        //エラーをコンソールに表示
        DebugOutputFormatString("[!!!ERROR!!!] Caught an unknown exception.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}