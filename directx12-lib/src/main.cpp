#include <Windows.h>

#include "Window.h"
#include <iostream>

#include "ResourceManager.h"

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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){

	try{
		//windows初期化処理
		WindowConfig winConf;
		winConf.appName = TEXT("DirectX12");
		winConf.x		= 0;
		winConf.y		= 0;
		winConf.width	= 1280;
		winConf.height	= 720;
		std::shared_ptr<Window> window = std::make_shared<Window>(winConf, hInstance, nCmdShow);
		window->init();
		ResourceManager::getInstance()->registerResource("window", window);

		//メッセージループ処理
		while(ResourceManager::getInstance()->getResource<Window>("window")->processMessages()) {

			//TODO:ここで描画処理を行う

		}

	}catch (const std::exception& e){
		//エラーをコンソールに表示
		DebugOutputFormatString("[!!!ERROR!!!] Caught an exception: %s", e.what());
		return EXIT_FAILURE;
	}
	catch(...){
		//エラーをコンソールに表示
		DebugOutputFormatString("[!!!ERROR!!!] Caught an unknown exception.");
	}

	return EXIT_SUCCESS;
}
