#pragma once
#include "WindowConfig.h"
/// <summary>
/// �E�B���h�E�쐬�N���X
/// </summary>
class Window
{
public:
	Window(WindowConfig winConf, HINSTANCE hInstance, int nCmdShow):
		hwnd(),
		winConf(winConf),
		hInstance(hInstance),
		nCmdShow(nCmdShow)

	{};
	~Window() {};

	//���b�Z�[�W���[�v ����
	bool processMessages();

	//������
	void init();

	//�I��
	void deinit();


private:
	//�E�B���h�E ����
	HWND create();

	//�E�B���h�E �\��
	void show();


private:
	//���������E�B���h�E�̃n���h��
	HWND hwnd;

	//�E�B���h�E�̐ݒ�
	WindowConfig winConf;

	//�C���X�^���X�̃n���h��
	HINSTANCE hInstance;

	//�E�B���h�E�̕\�����@���w�肷��l
	int nCmdShow;
};

