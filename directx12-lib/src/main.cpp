#include <Windows.h>

#include "Window.h"
#include <iostream>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){

	
	//windows����������
	WindowConfig winConf;
	winConf.appName = TEXT("DirectX12");
	winConf.x		= 0;
	winConf.y		= 0;
	winConf.width	= 1280;
	winConf.height	= 720;
	Window* window = new Window(winConf, hInstance, nCmdShow);
	window->init();

	//���b�Z�[�W���[�v����
	while(window->processMessages()) {

		//TODO:�����ŕ`�揈�����s��

	}

	//�I��
	window->deinit();

	//���������
	delete window;

	return 0;
}
