#pragma once
#include <Windows.h>
#include <memory>
#include <functional>

class DX12Resources;
class ImGuiManager;

/// <summary>
/// フレームワーククラス
/// </summary>
class Framework
{
    friend class FrameworkFactory;
private:
    Framework(const HWND& h) :
        hWnd_(h),
        dx12_resources_(),
        imgui_manager_()
    {}
public:
    ~Framework() {};

public:
    //実行
    int run(std::function<bool()> processMessages);
private:
    //初期化処理
    void init();
    //更新処理
    void update();
    //描画処理
    void render();
    //デバッグ描画処理
    void debugRender();
    //終了処理
    void deinit();

private:
    HWND hWnd_;                                     //ウィンドウハンドル
    std::shared_ptr<DX12Resources> dx12_resources_; //DX12リソース
    std::shared_ptr<ImGuiManager> imgui_manager_;   //ImGuiマネージャー
};
