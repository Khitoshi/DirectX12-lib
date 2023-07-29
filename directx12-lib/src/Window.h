#pragma once

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
        const TCHAR* app_name;	//アプリケーションの名前
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
    //初期化
    void init();
    //終了
    void deinit();

public:
    //ウィンドウハンドルの取得
    HWND getHWND() const { return hwnd_; };

private:
    //ウィンドウ 生成
    void create();
    //ウィンドウ 表示
    void show();

private:
    //ウィンドウの設定
    WindowConfig conf_;
    //生成したウィンドウのハンドル
    HWND hwnd_;
    //インスタンスのハンドル
    HINSTANCE hInstance_;
    //ウィンドウの表示方法を指定する値
    int nCmd_show_;
};
