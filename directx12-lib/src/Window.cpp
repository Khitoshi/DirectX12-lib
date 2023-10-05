#include <Windows.h>

#include "Window.h"
//#include "GraphicsConfigurator.h"
#include "InputManager.h"
#include "HighResolutionTimer.h"
#include <stdexcept>
#include "DX12Resources.h"
#ifdef _DEBUG
#include <imgui/imgui.h>
//imguiのウィンドウプロシージャを呼び出すための宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

/// <summary>
/// ウィンドウプロシージャ
/// ウィンドウメッセージをアプリケーション内で振り分けるための通関手続きを行う関数
/// つまりOSから送られてきたメッセージを処理するためのコールバック関数
/// </summary>
/// <param name="hwnd">メッセージが送信されたウィンドウのハンドル</param>
/// <param name="msg">送信されたメッセージ</param>
/// <param name="wparam">メッセージに関する付加情報</param>
/// <param name="lparam">メッセージに関する付加情報</param>
/// <returns>
/// メッセージに対する処理を行ったか
/// 行った　     :0
/// 行っていない :DefWIndowProcの戻り値
/// </returns>
LRESULT windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef _DEBUG
	//imguiのウィンドウプロシージャを呼び出す
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))return true;
#endif // _DEBUG
	// ウィンドウが破棄された場合アプリケーションを終了
	switch (msg) {
	case WM_MOUSEMOVE://マウスの移動
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);
		ScreenToClient(hwnd, &currentMousePos);
		InputManager::Instance().OnMouseMove(currentMousePos.x, currentMousePos.y);
		break;

		//TODO:マウスのボタンが押された時の処理を別の場所に移動する
	case WM_LBUTTONDOWN://マウスの左ボタンが押された
		InputManager::Instance().onMouseLeftButtonDown();
		break;
	case WM_LBUTTONUP://マウスの左ボタンが離された
		InputManager::Instance().onMouseLeftButtonUp();
		break;
	case WM_RBUTTONDOWN://マウスの右ボタンが押された
		InputManager::Instance().onMouseRightButtonDown();
		break;
	case WM_RBUTTONUP://マウスの右ボタンが離された
		InputManager::Instance().onMouseLeftButtonUp();
		break;

	case WM_ENTERSIZEMOVE://ウィンドウの位置変更開始
		HighResolutionTimer::getInstance().Stop();
		break;
	case WM_EXITSIZEMOVE://ウィンドウの位置変更終了
		HighResolutionTimer::getInstance().Start();
		break;
	case WM_SIZE://ウィンドウのサイズ変更
		//if (wparam == SIZE_MINIMIZED)/*最小化された*/ HighResolutionTimer::getInstance().Stop();
		//else if (wparam == SIZE_RESTORED)/*最小化から復帰*/HighResolutionTimer::getInstance().Start();
		//else if (wparam == SIZE_MAXIMIZED)/*最大化された*/HighResolutionTimer::getInstance().Start();

		//if (resouce) {
		//    RECT clientRect = {};
		//    GetClientRect(hwnd, &clientRect);
		//    resouce->OnSizeChanged(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wparam == SIZE_MINIMIZED);
		//}

		break;

	case WM_KEYDOWN:
		break;
	case WM_DESTROY:   //OSに終了を伝える
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:     //OSに終了を伝える
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	//規定の処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

/// <summary>
/// メッセージループ処理
/// </summary>
/// <returns>
/// true		: アプリケーション続行
/// false		: アプリケーション終了
/// </returns>
bool Window::processMessages() {
	MSG msg = { 0 };

	//アプリケーションが終わる時にmessageがWM_QUITになる
	while (msg.message != WM_QUIT) {
		//メッセージが存在するか確認，存在する場合msgに格納
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			//キーボード入力メッセージ（主にキーの押下とリリース）を文字メッセージに変換
			TranslateMessage(&msg);
			//取得したメッセージを対応するウィンドウプロシージャに送信
			DispatchMessage(&msg);
		}
		else {
			//ウィンドウメッセージが空になった
			break;
		}
	}

	return msg.message != WM_QUIT;
}

/// <summary>
/// 初期化
/// </summary>
void Window::init() {
	create();
	show();
}

/// <summary>
/// 終了
/// </summary>
void Window::deinit() {
	//ウィンドウクラスの登録解除
	UnregisterClass(this->conf_.app_name, this->hInstance_);
}

/// <summary>
/// ウィンドウ生成
/// </summary>
/// <returns>
/// 生成した新しいウィンドウへのハンドル
/// </returns>
void Window::create() {
	//windowクラスのパラメータ設定
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);		//構造体サイズ
	wc.style = CS_CLASSDC;				//ウィンドウのスタイル
	wc.lpfnWndProc = (WNDPROC)windowProcedure;	//ウィンドウプロシージャへのポインター
	wc.cbClsExtra = 0;						//構造体の後に割り当てる余分なバイト数
	wc.cbWndExtra = 0;						//インスタンスの後に割り当てる余分なバイト数
	wc.hInstance = GetModuleHandle(0);		//プロシージャを含むインスタンスへのハンドル
	wc.hIcon = NULL;						//クラスアイコンへのハンドル(NULL = defaultが表示)
	wc.hCursor = NULL;						//カーソルへのハンドル(NULL = ウィンドウに移動するたびにアプリケーションでカーソルの図形を明示的に設定する必要がある)
	wc.hbrBackground = NULL;						//背景ブラシへのハンドル(NULL = 独自背景が必要)
	wc.lpszMenuName = NULL;						//既定のメニューのマクロ(NULL = 規定メニュー無し)
	wc.lpszClassName = this->conf_.app_name;	//ウィンドウクラスの名前を指定
	wc.hIconSm = NULL;						//アイコンへのハンドル(NULL = 自動で適切なサイズのアイコンを検索)
	//ウィンドウクラスの登録
	RegisterClassEx(&wc);


	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	//RECT rect = { 0, 0, windowWidth, windowHeight };
	//AdjustWindowRect(&rect, dwStyle, FALSE);

	//TODO:マジックナンバーを使用しているので，GraphicsConfiguratorの情報を使用する方式に変更する

	//window生成 & 新しいウィンドウへのハンドルをreturn
	this->hwnd_ = CreateWindow(
		this->conf_.app_name,	//ウィンドウクラスの名前
		this->conf_.app_name,	//ウィンドウの名前,ウィンドウクラスの名前と別名でok
		dwStyle,	            //ウィンドウスタイル
		this->conf_.x,		    //ウィンドウ表示位置:x
		this->conf_.y,		    //ウィンドウ表示位置:y
		1280,	        //ウィンドウのサイズ:幅
		720,	        //ウィンドウのサイズ:高さ
		NULL,					//親ウィンドウのハンドル
		NULL,					//メニューのハンドル
		this->hInstance_,		//インスタンスのハンドル
		NULL					//作成時の引数保存用ポインタ
	);

	//ウィンドウハンドル生成チェック
	if (this->hwnd_ == NULL) {
		throw std::runtime_error("ウィンドウ生成失敗");
	}
}

/// <summary>
/// ウィンドウ 表示
/// </summary>
void Window::show() {
	//ウィンドウ表示
	ShowWindow(this->hwnd_, this->nCmd_show_);
}