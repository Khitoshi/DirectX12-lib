#pragma once

#include <string>

/// <summary>
/// ウィンドウ作成クラス
/// </summary>
class Window
{
public:
	struct WindowConfig {
		std::string app_name;	//アプリケーションの名前
		UINT x;					//windowの位置:x軸
		UINT y;					//windowの位置:y軸
	};

public:
	Window(WindowConfig winConf, HINSTANCE hInstance, int nCmdShow) :
		hwnd_(),
		conf_(winConf),
		hInstance_(hInstance),
		nCmd_show_(nCmdShow)

	{};
	~Window() { deinit(); };

	bool processMessages();
	void init();
	void deinit();

public://取得系
	HWND getHWND() const { return hwnd_; };

private:
	void create();
	void show();

private:
	WindowConfig conf_;
	HWND hwnd_;
	HINSTANCE hInstance_;
	int nCmd_show_;
};
