#include "Framework.h"
#include "SceneManager.h"
#include "DX12Resources.h"
#include "imgui/ImGuiManager.h"

#include "DeviceContext.h"

#include "HighResolutionTimer.h"

#include <exception>
#include <iostream>
#include <sstream>

/// <summary>
/// デバッグ用出力関数
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
/// 実行処理
/// </summary>
/// <param name="processMessages">メッセージ処理ループ</param>
/// <returns>
/// 正常終了: EXIT_SUCCESSを返す
/// エラー : EXIT_FAILUREを返す
/// </returns>
int Framework::run(std::function<bool()> processMessages)
{
    MSG msg = {};
    try {
        //アプリケーションが終わる時にmessageがWM_QUITになる
        while (processMessages()) {

            HighResolutionTimer::getInstance().Tick();
            const float elapsed_time_ = HighResolutionTimer::getInstance().TimeInterval();

            calculateFrameStats();
            this->update();
            this->render();
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
    this->deinit();

    return EXIT_SUCCESS;
}

/// <summary>
/// 更新処理
/// </summary>
void Framework::init()
{
    //DX12初期化処理
    this->dx12_resources_ = std::make_shared<DX12Resources>();
    this->dx12_resources_->init(this->hWnd_);

    //imgui初期化処理
    this->imgui_manager_ = std::make_shared<ImGuiManager>();
    this->imgui_manager_->init(this->dx12_resources_->getDeviceContext()->getDevice(), this->hWnd_);

    //シーン登録処理
    //SceneManager::getInstance().registerScene();
    SceneManager::getInstance().init(this->dx12_resources_->getDeviceContext()->getDevice(), std::make_shared<SceneTriangle>());
}

/// <summary>
/// 更新処理
/// </summary>
void Framework::update()
{

    this->dx12_resources_->beginRender();

    SceneManager::getInstance().update();
}

/// <summary>
/// メイン描画処理
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
/// デバッグ用描画処理
/// </summary>
void Framework::debugRender()
{
    //imguiFrame開始処理
    auto render_context = this->dx12_resources_->getRenderContext();
    auto device_context = this->dx12_resources_->getDeviceContext();

    this->imgui_manager_->beginFrame(render_context, device_context->getDevice());
    ImGui::Begin("System");

    //マウス座標表示
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImGui::Text("Mouse Position: (%.1f,%.1f)", mousePos.x, mousePos.y);

    //FPS表示
    //ImGui::Text("FrameRate: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("FPS: %.f / FRAME TIME: %.f ms", this->fps_, this->mspf_);

    auto gpu_info = device_context->getGPUInfo();
    ImGui::Text("Adaptor : %ls", gpu_info.name_.c_str());
    ImGui::Text("Dedicated Video Memory: %d", gpu_info.dedicated_video_memory_);
    ImGui::Text("Dedicated System Memory: %d", gpu_info.dedicated_system_memory_);
    ImGui::Text("Shared System Memory: %d", gpu_info.shared_system_memory_);

    ImGui::End();

    //imguiメニュー更新処理
    SceneManager::getInstance().updateImguiMenu();
    //シーン選択
    SceneManager::getInstance().sceneSelect();

    //Frame終了処理
    this->imgui_manager_->endFrame();
    //imgui描画処理
    this->imgui_manager_->render(render_context, device_context->getDevice());

}

/// <summary>
/// 終了処理
/// </summary>
void Framework::deinit()
{
    this->imgui_manager_->deinit();
    this->dx12_resources_->waitEndOfDrawing();
}


/// <summary>
/// フレームレート計算処理
/// </summary>
void Framework::calculateFrameStats()
{
    static int frames = 0;
    static float time_tlapsed = 0.0f;

    frames++;

    if ((HighResolutionTimer::getInstance().TimeStamp() - time_tlapsed) >= 1.0f)
    {
        float fps = (float)frames;
        float mspf = 1000.0f / fps;
        this->fps_ = fps;
        this->mspf_ = mspf;

#if _DEBUG
        std::ostringstream outs;
        outs.precision(6);
        outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
        SetWindowTextA(this->hWnd_, outs.str().c_str());
#endif

        frames = 0;
        time_tlapsed += 1.0f;
    }
}
