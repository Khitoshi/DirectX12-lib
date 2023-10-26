#include <Windows.h>
#include "Window.h"
#include "FrameworkFactory.h"

#include <memory>
#include "GraphicsConfigurator.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	GraphicsConfigurator::getInstance().init();

	Window::WindowConfig winConf = {};

	winConf.app_name = GraphicsConfigurator::getInstance().getConfigurationData().app_name;
	winConf.x = 0;
	winConf.y = 0;
	std::shared_ptr<Window> window = std::make_shared<Window>(winConf, hInstance, nCmdShow);
	window->init();

	std::shared_ptr<Framework> f = FrameworkFactory::create(window->getHWND());
	SetWindowLongPtr(window->getHWND(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
	return f->run([window]() { return window->processMessages(); });
}
