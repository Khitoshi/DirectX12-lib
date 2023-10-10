#pragma once

#include <string>

/// <summary>
/// ウィンドウ作成クラス
/// </summary>
class Window
{
public:
	/// <summary>
	/// windowの設定
	/// </summary>
	struct WindowConfig {
		//const WCHAR* app_name;	//アプリケーションの名前
		std::string app_name;	//アプリケーションの名前
		UINT x;					//windowの位置:x軸
		UINT y;					//windowの位置:y軸
	};

public:
	/// <summary>
	/// </summary>
	/// <param name="winConf">ウィンドウの設定</param>
	/// <param name="hInstance">インスタンスのハンドル</param>
	/// <param name="nCmdShow">ウィンドウの表示方法を指定する値</param>
	Window(WindowConfig winConf, HINSTANCE hInstance, int nCmdShow) :
		hwnd_(),
		conf_(winConf),
		hInstance_(hInstance),
		nCmd_show_(nCmdShow)

	{};
	~Window() { deinit(); };

	//メッセージループ 処理
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
