#pragma once

#include <string>

/// <summary>
/// �E�B���h�E�쐬�N���X
/// </summary>
class Window
{
public:
	/// <summary>
	/// window�̐ݒ�
	/// </summary>
	struct WindowConfig {
		//const WCHAR* app_name;	//�A�v���P�[�V�����̖��O
		std::string app_name;	//�A�v���P�[�V�����̖��O
		UINT x;					//window�̈ʒu:x��
		UINT y;					//window�̈ʒu:y��
	};

public:
	/// <summary>
	/// </summary>
	/// <param name="winConf">�E�B���h�E�̐ݒ�</param>
	/// <param name="hInstance">�C���X�^���X�̃n���h��</param>
	/// <param name="nCmdShow">�E�B���h�E�̕\�����@���w�肷��l</param>
	Window(WindowConfig winConf, HINSTANCE hInstance, int nCmdShow) :
		hwnd_(),
		conf_(winConf),
		hInstance_(hInstance),
		nCmd_show_(nCmdShow)

	{};
	~Window() { deinit(); };

	//���b�Z�[�W���[�v ����
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
