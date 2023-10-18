#include "Framework.h"
#include "SceneManager.h"
#include "DX12Resources.h"
#include "ImGuiManager.h"

#include "DeviceContext.h"

#include "HighResolutionTimer.h"

#include <exception>
#include <iostream>
#include <sstream>

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

int Framework::run(std::function<bool()> processMessages)
{
	MSG msg = {};
	try {
		//アプリケーションが終わる時にmessageがWM_QUITになる
		while (processMessages()) {

			HighResolutionTimer::getInstance().Tick();
			const float elapsed_time_ = HighResolutionTimer::getInstance().TimeInterval();

			//calculateFrameStats();
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

void Framework::setDirectoryAndDll()
{
#ifdef _DEBUG
	SetDllDirectoryA("lib/assimp/lib/Debug");
	LoadLibraryExA("assimp-vc143-mtd.dll", NULL, NULL);

#else
	SetDllDirectoryA("lib/assimp/lib/Release");
	LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);
#endif // _DEBUG

}

void Framework::init()
{
	setDirectoryAndDll();

	//DX12初期化処理
	this->dx12_resources_ = std::make_shared<DX12Resources>();
	this->dx12_resources_->init(this->hWnd_);

	//imgui初期化処理
	this->imgui_manager_ = std::make_shared<ImGuiManager>(&this->hWnd_);
	this->imgui_manager_->init(this->dx12_resources_->getDeviceContext()->getDevice());

	//シーン登録処理
	//SceneManager::getInstance().registerScene();
	//SceneManager::getInstance().init(this->dx12_resources_->getDeviceContext()->getDevice(), std::make_shared<SceneModel>());
	SceneManager::getInstance().init(this->dx12_resources_->getDeviceContext()->getDevice(), std::make_shared<SceneTriangle>());
}

void Framework::update()
{

	SceneManager::getInstance().update();
}

void Framework::render()
{
	this->dx12_resources_->beginRender();
	debugRender();
	SceneManager::getInstance().render(this->dx12_resources_->getRenderContext());

	this->dx12_resources_->endRender();

	auto device_context = this->dx12_resources_->getDeviceContext();

	SceneManager::getInstance().changeScene(device_context->getDevice());
}


void Framework::debugRender()
{
	//imguiFrame開始処理
	auto render_context = this->dx12_resources_->getRenderContext();
	auto device_context = this->dx12_resources_->getDeviceContext();

	this->imgui_manager_->beginRender(render_context, device_context->getDevice());
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

	/*
	if (ImGui::Button("FULLSCREEN")) {
		this->dx12_resources_->setWindowSizeChanged(true);
	}
	*/

	ImGui::End();

	//imguiメニュー更新処理
	SceneManager::getInstance().updateImguiMenu(render_context, this->imgui_manager_.get());
	//シーン選択
	SceneManager::getInstance().sceneSelect();

	//imgui描画処理
	this->imgui_manager_->endRender(render_context, device_context->getDevice());

}

void Framework::deinit()
{
	this->imgui_manager_->deinit();
	this->dx12_resources_->deinit();
}

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
