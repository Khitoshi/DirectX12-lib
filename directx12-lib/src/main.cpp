#include <Windows.h>

#include "Window.h"
#include <iostream>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){

	
	//windows初期化処理
	WindowConfig winConf;
	winConf.appName = TEXT("DirectX12");
	winConf.x		= 0;
	winConf.y		= 0;
	winConf.width	= 1280;
	winConf.height	= 720;
	Window* window = new Window(winConf, hInstance, nCmdShow);
	window->init();

	//メッセージループ処理
	while(window->processMessages()) {

		//TODO:ここで描画処理を行う

	}

	//終了
	window->deinit();

	//メモリ解放
	delete window;

	return 0;
}
