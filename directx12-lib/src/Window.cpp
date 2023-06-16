#include <Windows.h>

#include "Window.h"
#include <stdexcept>


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
LRESULT windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){
    // �E�B���h�E���j�����ꂽ�ꍇ�A�v���P�[�V�������I��
    if (msg == WM_DESTROY){
        //OS�ɏI����`����
        PostQuitMessage(0);
        return 0;
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
bool Window::processMessages(){
	MSG msg = { 0 };

	//�A�v���P�[�V�������I��鎞��message��WM_QUIT�ɂȂ�
	while (msg.message != WM_QUIT){
		//���b�Z�[�W�����݂��邩�m�F�C���݂���ꍇmsg�Ɋi�[
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			//�L�[�{�[�h���̓��b�Z�[�W�i��ɃL�[�̉����ƃ����[�X�j�𕶎����b�Z�[�W�ɕϊ�
			TranslateMessage(&msg);
			//�擾�������b�Z�[�W��Ή�����E�B���h�E�v���V�[�W���ɑ��M
			DispatchMessage(&msg);
		}else{
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
	this->hwnd = create();

	show();
}



/// <summary>
/// �I��
/// </summary>
void Window::deinit() {
	//�E�B���h�E�N���X�̓o�^����
	UnregisterClass(this->winConf.appName, this->hInstance);
}



/// <summary>
/// �E�B���h�E����
/// </summary>
/// <returns>
/// ���������V�����E�B���h�E�ւ̃n���h��
/// </returns>
HWND Window::create()
{
	//window�N���X�̃p�����[�^�ݒ�
	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);		//�\���̃T�C�Y
	wc.style			= CS_CLASSDC;				//�E�B���h�E�̃X�^�C��
	wc.lpfnWndProc		= (WNDPROC)windowProcedure;	//�E�B���h�E�v���V�[�W���ւ̃|�C���^�[
	wc.cbClsExtra		= 0;						//�\���̂̌�Ɋ��蓖�Ă�]���ȃo�C�g��
	wc.cbWndExtra		= 0;						//�C���X�^���X�̌�Ɋ��蓖�Ă�]���ȃo�C�g��
	wc.hInstance		= GetModuleHandle(0);		//�v���V�[�W�����܂ރC���X�^���X�ւ̃n���h��
	wc.hIcon			= NULL;						//�N���X�A�C�R���ւ̃n���h��(NULL = default���\��)
	wc.hCursor			= NULL;						//�J�[�\���ւ̃n���h��(NULL = �E�B���h�E�Ɉړ����邽�тɃA�v���P�[�V�����ŃJ�[�\���̐}�`�𖾎��I�ɐݒ肷��K�v������)
	wc.hbrBackground	= NULL;						//�w�i�u���V�ւ̃n���h��(NULL = �Ǝ��w�i���K�v)
	wc.lpszMenuName		= NULL;						//����̃��j���[�̃}�N��(NULL = �K�胁�j���[����)
	wc.lpszClassName	= this->winConf.appName;	//�E�B���h�E�N���X�̖��O���w��
	wc.hIconSm			= NULL;						//�A�C�R���ւ̃n���h��(NULL = �����œK�؂ȃT�C�Y�̃A�C�R��������)

	//�E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wc);

	//window���� & �V�����E�B���h�E�ւ̃n���h����return
	hwnd = CreateWindow(
		this->winConf.appName,	//�E�B���h�E�N���X�̖��O
		this->winConf.appName,	//�E�B���h�E�̖��O,�E�B���h�E�N���X�̖��O�ƕʖ���ok
		WS_OVERLAPPEDWINDOW,	//�E�B���h�E�X�^�C��
		this->winConf.x,		//�E�B���h�E�\���ʒu:x
		this->winConf.y,		//�E�B���h�E�\���ʒu:y
		this->winConf.width,	//�E�B���h�E�̃T�C�Y:��
		this->winConf.height,	//�E�B���h�E�̃T�C�Y:����
		NULL,					//�e�E�B���h�E�̃n���h��
		NULL,					//���j���[�̃n���h��
		this->hInstance,		//�C���X�^���X�̃n���h��
		NULL					//�쐬���̈����ۑ��p�|�C���^
	);

	//�E�B���h�E�n���h�������`�F�b�N
	if (this->hwnd == NULL) {
		throw std::runtime_error("�E�B���h�E�������s");
	}

	return hwnd;
}



/// <summary>
/// �E�B���h�E �\��
/// </summary>
void Window::show()
{
	//�E�B���h�E�\��
	ShowWindow(this->hwnd, this->nCmdShow);
}



