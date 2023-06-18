#include "Shader.h"
#include <d3dcompiler.h>
#include <stdexcept>
#include <stierr.h>
#include <sstream>
#include <fstream>
#include <atlbase.h>


/// <summary>
/// ���_�V�F�[�_�[�̓ǂݍ���
/// </summary>
/// <param name="filePath">�t�@�C���p�X</param>
/// <param name="entryFuncName">�V�F�[�_�[�G���g���[�֐���</param>
void Shader::LoadVS(const char* filePath, const char* entryFuncName)
{
    this->shaderBlob = load(filePath, entryFuncName, "vs_5_0");
}


/// <summary>
/// �s�N�Z���V�F�[�_�[�̓ǂݍ���
/// </summary>
/// <param name="filePath">�t�@�C���p�X</param>
/// <param name="entryFuncName">�V�F�[�_�[�G���g���[�֐���</param>
void Shader::LoadPS(const char* filePath, const char* entryFuncName)
{
    this->shaderBlob = load(filePath, entryFuncName, "ps_5_0");
}


/// <summary>
/// �V�F�[�_�[�̓ǂݍ���
/// </summary>
/// <param name="filePath">�V�F�[�_�[�t�@�C���p�X</param>
/// <param name="entryFuncName">�V�F�[�_�[�G���g���[�֐���</param>
/// <param name="shaderModel">�V�F�[�_�[�o�[�W����</param>
/// <returns>
/// ����ɃR���p�C�����ꂽ�V�F�[�_�[
/// </returns>
ComPtr<ID3D10Blob> Shader::load(const char* filePath, const char* entryFuncName, const char* shaderModel)
{
    ID3DBlob* errorBlob;
#ifdef _DEBUG
    //�V�F�[�_�[�̃f�o�b�O��L���ɂ���
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    //�t�@�C���p�X�̕ϊ�
    wchar_t wfxFilePath[256] = { L"" };
    //mbstowcs(wfxFilePath, filePath, 256);
    mbstowcs_s(nullptr, wfxFilePath, filePath, 256);

    //�V�F�[�_�[�̃R���p�C��
    ComPtr<ID3D10Blob> shaderBlob;

    HRESULT hr = D3DCompileFromFile(wfxFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "Main", entryFuncName, compileFlags, 0, shaderBlob.GetAddressOf(), &errorBlob);
    if (FAILED(hr)) {
        if (hr == STIERR_OBJECTNOTFOUND) {
            throw "FAILED shader load error: file not found";
        }
        if (errorBlob != nullptr) {
            throw "FAILED shader load error: " + std::string((char*)errorBlob->GetBufferPointer());
        }
    }

    return shaderBlob;
}