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
		imgui_manager_(),
		fps_(0),
		mspf_(0)
	{}
public:
	~Framework() {};

public:
	//実行
	int run(std::function<bool()> processMessages);
private:
	void setDirectoryAndDll();
	void init();
	void update();
	void render();
	void debugRender();
	void deinit();

	void calculateFrameStats();

private:
	HWND hWnd_;
	std::shared_ptr<DX12Resources> dx12_resources_;
	std::shared_ptr<ImGuiManager> imgui_manager_;
	float fps_;
	float mspf_;
};
