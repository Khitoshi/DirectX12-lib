#pragma once
#include <Windows.h>
#include <memory>
#include <functional>

class DX12Resources;
class ImGuiManager;

/// <summary>
/// �t���[�����[�N�N���X
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
	//���s
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
