#include <Windows.h>

#include "Window.h"
//#include "GraphicsConfigurator.h"
#include "InputManager.h"
#include "HighResolutionTimer.h"
#include <stdexcept>
#include "DX12Resources.h"
#ifdef _DEBUG
#include <imgui/imgui.h>
//imgui�̃E�B���h�E�v���V�[�W�����Ăяo�����߂̐錾
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

/// <summary>
/// �E�B���h�E�v���V�[�W��
/// �E�B���h�E���b�Z�[�W���A�v���P�[�V�������ŐU�蕪���邽�߂̒ʊ֎葱�����s���֐�
/// �܂�OS���瑗���Ă������b�Z�[�W���������邽�߂̃R�[���o�b�N�֐�
/// </summary>
/// <param name="hwnd">���b�Z�[�W�����M���ꂽ�E�B���h�E�̃n���h��</param>
/// <param name="msg">���M���ꂽ���b�Z�[�W</param>
/// <param name="wparam">���b�Z�[�W�Ɋւ���t�����</param>
/// <param name="lparam">���b�Z�[�W�Ɋւ���t�����</param>
/// <returns>
/// ���b�Z�[�W�ɑ΂��鏈�����s������
/// �s�����@     :0
/// �s���Ă��Ȃ� :DefWIndowProc�̖߂�l
/// </returns>
LRESULT windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef _DEBUG
	//imgui�̃E�B���h�E�v���V�[�W�����Ăяo��
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))return true;
#endif // _DEBUG
	// �E�B���h�E���j�����ꂽ�ꍇ�A�v���P�[�V�������I��
	switch (msg) {
	case WM_MOUSEMOVE://�}�E�X�̈ړ�
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);
		ScreenToClient(hwnd, &currentMousePos);
		InputManager::Instance().OnMouseMove(currentMousePos.x, currentMousePos.y);
		break;

		//TODO:�}�E�X�̃{�^���������ꂽ���̏�����ʂ̏ꏊ�Ɉړ�����
	case WM_LBUTTONDOWN://�}�E�X�̍��{�^���������ꂽ
		InputManager::Instance().onMouseLeftButtonDown();
		break;
	case WM_LBUTTONUP://�}�E�X�̍��{�^���������ꂽ
		InputManager::Instance().onMouseLeftButtonUp();
		break;
	case WM_RBUTTONDOWN://�}�E�X�̉E�{�^���������ꂽ
		InputManager::Instance().onMouseRightButtonDown();
		break;
	case WM_RBUTTONUP://�}�E�X�̉E�{�^���������ꂽ
		InputManager::Instance().onMouseLeftButtonUp();
		break;

	case WM_ENTERSIZEMOVE://�E�B���h�E�̈ʒu�ύX�J�n
		HighResolutionTimer::getInstance().Stop();
		break;
	case WM_EXITSIZEMOVE://�E�B���h�E�̈ʒu�ύX�I��
		HighResolutionTimer::getInstance().Start();
		break;
	case WM_SIZE://�E�B���h�E�̃T�C�Y�ύX
		//if (wparam == SIZE_MINIMIZED)/*�ŏ������ꂽ*/ HighResolutionTimer::getInstance().Stop();
		//else if (wparam == SIZE_RESTORED)/*�ŏ������畜�A*/HighResolutionTimer::getInstance().Start();
		//else if (wparam == SIZE_MAXIMIZED)/*�ő剻���ꂽ*/HighResolutionTimer::getInstance().Start();

		//if (resouce) {
		//    RECT clientRect = {};
		//    GetClientRect(hwnd, &clientRect);
		//    resouce->OnSizeChanged(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wparam == SIZE_MINIMIZED);
		//}

		break;

	case WM_KEYDOWN:
		break;
	case WM_DESTROY:   //OS�ɏI����`����
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:     //OS�ɏI����`����
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	//�K��̏������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

/// <summary>
/// ���b�Z�[�W���[�v����
/// </summary>
/// <returns>
/// true		: �A�v���P�[�V�������s
/// false		: �A�v���P�[�V�����I��
/// </returns>
bool Window::processMessages() {
	MSG msg = { 0 };

	//�A�v���P�[�V�������I��鎞��message��WM_QUIT�ɂȂ�
	while (msg.message != WM_QUIT) {
		//���b�Z�[�W�����݂��邩�m�F�C���݂���ꍇmsg�Ɋi�[
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			//�L�[�{�[�h���̓��b�Z�[�W�i��ɃL�[�̉����ƃ����[�X�j�𕶎����b�Z�[�W�ɕϊ�
			TranslateMessage(&msg);
			//�擾�������b�Z�[�W��Ή�����E�B���h�E�v���V�[�W���ɑ��M
			DispatchMessage(&msg);
		}
		else {
			//�E�B���h�E���b�Z�[�W����ɂȂ���
			break;
		}
	}

	return msg.message != WM_QUIT;
}

/// <summary>
/// ������
/// </summary>
void Window::init() {
	create();
	show();
}

/// <summary>
/// �I��
/// </summary>
void Window::deinit() {
	//�E�B���h�E�N���X�̓o�^����
	UnregisterClass(this->conf_.app_name, this->hInstance_);
}

/// <summary>
/// �E�B���h�E����
/// </summary>
/// <returns>
/// ���������V�����E�B���h�E�ւ̃n���h��
/// </returns>
void Window::create() {
	//window�N���X�̃p�����[�^�ݒ�
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);		//�\���̃T�C�Y
	wc.style = CS_CLASSDC;				//�E�B���h�E�̃X�^�C��
	wc.lpfnWndProc = (WNDPROC)windowProcedure;	//�E�B���h�E�v���V�[�W���ւ̃|�C���^�[
	wc.cbClsExtra = 0;						//�\���̂̌�Ɋ��蓖�Ă�]���ȃo�C�g��
	wc.cbWndExtra = 0;						//�C���X�^���X�̌�Ɋ��蓖�Ă�]���ȃo�C�g��
	wc.hInstance = GetModuleHandle(0);		//�v���V�[�W�����܂ރC���X�^���X�ւ̃n���h��
	wc.hIcon = NULL;						//�N���X�A�C�R���ւ̃n���h��(NULL = default���\��)
	wc.hCursor = NULL;						//�J�[�\���ւ̃n���h��(NULL = �E�B���h�E�Ɉړ����邽�тɃA�v���P�[�V�����ŃJ�[�\���̐}�`�𖾎��I�ɐݒ肷��K�v������)
	wc.hbrBackground = NULL;						//�w�i�u���V�ւ̃n���h��(NULL = �Ǝ��w�i���K�v)
	wc.lpszMenuName = NULL;						//����̃��j���[�̃}�N��(NULL = �K�胁�j���[����)
	wc.lpszClassName = this->conf_.app_name;	//�E�B���h�E�N���X�̖��O���w��
	wc.hIconSm = NULL;						//�A�C�R���ւ̃n���h��(NULL = �����œK�؂ȃT�C�Y�̃A�C�R��������)
	//�E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wc);


	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	//RECT rect = { 0, 0, windowWidth, windowHeight };
	//AdjustWindowRect(&rect, dwStyle, FALSE);

	//TODO:�}�W�b�N�i���o�[���g�p���Ă���̂ŁCGraphicsConfigurator�̏����g�p��������ɕύX����

	//window���� & �V�����E�B���h�E�ւ̃n���h����return
	this->hwnd_ = CreateWindow(
		this->conf_.app_name,	//�E�B���h�E�N���X�̖��O
		this->conf_.app_name,	//�E�B���h�E�̖��O,�E�B���h�E�N���X�̖��O�ƕʖ���ok
		dwStyle,	            //�E�B���h�E�X�^�C��
		this->conf_.x,		    //�E�B���h�E�\���ʒu:x
		this->conf_.y,		    //�E�B���h�E�\���ʒu:y
		1280,	        //�E�B���h�E�̃T�C�Y:��
		720,	        //�E�B���h�E�̃T�C�Y:����
		NULL,					//�e�E�B���h�E�̃n���h��
		NULL,					//���j���[�̃n���h��
		this->hInstance_,		//�C���X�^���X�̃n���h��
		NULL					//�쐬���̈����ۑ��p�|�C���^
	);

	//�E�B���h�E�n���h�������`�F�b�N
	if (this->hwnd_ == NULL) {
		throw std::runtime_error("�E�B���h�E�������s");
	}
}

/// <summary>
/// �E�B���h�E �\��
/// </summary>
void Window::show() {
	//�E�B���h�E�\��
	ShowWindow(this->hwnd_, this->nCmd_show_);
}