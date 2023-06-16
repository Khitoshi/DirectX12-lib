#pragma once
#include "WindowConfig.h"
/// <summary>
/// ウィンドウ作成クラス
/// </summary>
class Window
{
public:
	/// <summary>
	/// </summary>
	/// <param name="winConf">ウィンドウの設定</param>
	/// <param name="hInstance">インスタンスのハンドル</param>
	/// <param name="nCmdShow">ウィンドウの表示方法を指定する値</param>
	Window(WindowConfig winConf, HINSTANCE hInstance, int nCmdShow):
		hwnd(),
		winConf(winConf),
		hInstance(hInstance),
		nCmdShow(nCmdShow)

	{};
	~Window() { deinit(); };

	//メッセージループ 処理
	bool processMessages();

	//初期化
	void init();

	//終了
	void deinit();


private:
	//ウィンドウ 生成
	HWND create();

	//ウィンドウ 表示
	void show();


private:
	//生成したウィンドウのハンドル
	HWND hwnd;

	//ウィンドウの設定
	WindowConfig winConf;

	//インスタンスのハンドル
	HINSTANCE hInstance;

	//ウィンドウの表示方法を指定する値
	int nCmdShow;
};

