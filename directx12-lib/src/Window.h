#pragma once

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
        const TCHAR* app_name;	//�A�v���P�[�V�����̖��O
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
    //������
    void init();
    //�I��
    void deinit();

public:
    //�E�B���h�E�n���h���̎擾
    HWND getHWND() const { return hwnd_; };

private:
    //�E�B���h�E ����
    void create();
    //�E�B���h�E �\��
    void show();

private:
    //�E�B���h�E�̐ݒ�
    WindowConfig conf_;
    //���������E�B���h�E�̃n���h��
    HWND hwnd_;
    //�C���X�^���X�̃n���h��
    HINSTANCE hInstance_;
    //�E�B���h�E�̕\�����@���w�肷��l
    int nCmd_show_;
};
