#pragma once

#include <string>

/// <summary>
/// �E�B���h�E�쐬�N���X
/// </summary>
class Window
{
public:
	struct WindowConfig {
		std::string app_name;	//�A�v���P�[�V�����̖��O
		UINT x;					//window�̈ʒu:x��
		UINT y;					//window�̈ʒu:y��
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

public://�擾�n
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
