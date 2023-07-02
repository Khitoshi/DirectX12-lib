#include "Shader.h"
#include <d3dcompiler.h>
#include <stdexcept>
#include <stierr.h>
#include <sstream>
#include <fstream>
#include <atlbase.h>

/// <summary>
/// �V�F�[�_�[�̓ǂݍ���
/// </summary>
/// <param name="filePath">�V�F�[�_�[�t�@�C���p�X</param>
/// <param name="entryFuncName">�V�F�[�_�[�G���g���[�֐���</param>
/// <param name="shaderModel">�V�F�[�_�[�o�[�W����</param>
/// <returns>
/// ����ɃR���p�C�����ꂽ�V�F�[�_�[
/// </returns>
void Shader::load(ShaderConf conf)
{
#ifdef _DEBUG
    //�V�F�[�_�[�̃f�o�b�O��L���ɂ���
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    //�t�@�C���p�X�̕ϊ�
    wchar_t wfxFilePath[256] = { L"" };
    //mbstowcs(wfxFilePath, filePath, 256);
    mbstowcs_s(nullptr, wfxFilePath, conf.filePath.c_str(), 256);

    //�V�F�[�_�[�̃R���p�C��
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        wfxFilePath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        conf.entryFuncName.c_str(),
        conf.ShaderModelNames[(int)conf.currentShaderModelType].c_str(),
        compileFlags,
        0,
        this->shaderBlob.GetAddressOf(),
        &errorBlob);
    if (FAILED(hr)) {
        if (hr == STIERR_OBJECTNOTFOUND) {
            throw "FAILED shader load error: file not found";
        }
        if (errorBlob) {
            throw "FAILED shader load error: " + std::string((char*)errorBlob->GetBufferPointer());
        }
        else {
            throw "FAILED shader load error: unknown error";
        }
    }
    else if (!shaderBlob) {
        throw "FAILED shader load error: shaderBlob is nullptr";
    }
}