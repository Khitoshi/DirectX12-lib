#include <Windows.h>

#include "Window.h"
#include <stdexcept>


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
LRESULT windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){
    // ウィンドウが破棄された場合アプリケーションを終了
    if (msg == WM_DESTROY){
        //OSに終了を伝える
        PostQuitMessage(0);
        return 0;
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
bool Window::processMessages(){
	MSG msg = { 0 };

	//アプリケーションが終わる時にmessageがWM_QUITになる
	while (msg.message != WM_QUIT){
		//メッセージが存在するか確認，存在する場合msgに格納
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			//キーボード入力メッセージ（主にキーの押下とリリース）を文字メッセージに変換
			TranslateMessage(&msg);
			//取得したメッセージを対応するウィンドウプロシージャに送信
			DispatchMessage(&msg);
		}else{
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
	this->hwnd = create();

	show();
}



/// <summary>
/// 終了
/// </summary>
void Window::deinit() {
	//ウィンドウクラスの登録解除
	UnregisterClass(this->winConf.appName, this->hInstance);
}



/// <summary>
/// ウィンドウ生成
/// </summary>
/// <returns>
/// 生成した新しいウィンドウへのハンドル
/// </returns>
HWND Window::create()
{
	//windowクラスのパラメータ設定
	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);		//構造体サイズ
	wc.style			= CS_CLASSDC;				//ウィンドウのスタイル
	wc.lpfnWndProc		= (WNDPROC)windowProcedure;	//ウィンドウプロシージャへのポインター
	wc.cbClsExtra		= 0;						//構造体の後に割り当てる余分なバイト数
	wc.cbWndExtra		= 0;						//インスタンスの後に割り当てる余分なバイト数
	wc.hInstance		= GetModuleHandle(0);		//プロシージャを含むインスタンスへのハンドル
	wc.hIcon			= NULL;						//クラスアイコンへのハンドル(NULL = defaultが表示)
	wc.hCursor			= NULL;						//カーソルへのハンドル(NULL = ウィンドウに移動するたびにアプリケーションでカーソルの図形を明示的に設定する必要がある)
	wc.hbrBackground	= NULL;						//背景ブラシへのハンドル(NULL = 独自背景が必要)
	wc.lpszMenuName		= NULL;						//既定のメニューのマクロ(NULL = 規定メニュー無し)
	wc.lpszClassName	= this->winConf.appName;	//ウィンドウクラスの名前を指定
	wc.hIconSm			= NULL;						//アイコンへのハンドル(NULL = 自動で適切なサイズのアイコンを検索)

	//ウィンドウクラスの登録
	RegisterClassEx(&wc);

	//window生成 & 新しいウィンドウへのハンドルをreturn
	hwnd = CreateWindow(
		this->winConf.appName,	//ウィンドウクラスの名前
		this->winConf.appName,	//ウィンドウの名前,ウィンドウクラスの名前と別名でok
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
		this->winConf.x,		//ウィンドウ表示位置:x
		this->winConf.y,		//ウィンドウ表示位置:y
		this->winConf.width,	//ウィンドウのサイズ:幅
		this->winConf.height,	//ウィンドウのサイズ:高さ
		NULL,					//親ウィンドウのハンドル
		NULL,					//メニューのハンドル
		this->hInstance,		//インスタンスのハンドル
		NULL					//作成時の引数保存用ポインタ
	);

	//ウィンドウハンドル生成チェック
	if (this->hwnd == NULL) {
		throw std::runtime_error("ウィンドウ生成失敗");
	}

	return hwnd;
}



/// <summary>
/// ウィンドウ 表示
/// </summary>
void Window::show()
{
	//ウィンドウ表示
	ShowWindow(this->hwnd, this->nCmdShow);
}



