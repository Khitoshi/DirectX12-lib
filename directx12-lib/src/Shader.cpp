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
	UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compile_flags = 0;
#endif

	//�t�@�C���p�X�̕ϊ�
	wchar_t wfxFilePath[256] = { L"" };
	//mbstowcs(wfxFilePath, filePath, 256);
	mbstowcs_s(nullptr, wfxFilePath, conf.file_path.c_str(), 256);

	//�V�F�[�_�[�̃R���p�C��
	ComPtr<ID3DBlob> err_blob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		wfxFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		conf.entry_func_name.c_str(),
		conf.ShaderModelNames[(int)conf.current_shader_model_type].c_str(),
		compile_flags,
		0,
		this->shader_blob_.GetAddressOf(),
		&err_blob);
	if (FAILED(hr)) {
		if (hr == STIERR_OBJECTNOTFOUND) {
			throw "FAILED shader load error: file not found";
		}
		if (err_blob) {
			auto errorstr = std::string((char*)err_blob->GetBufferPointer());
			throw "FAILED shader load error: " + std::string((char*)err_blob->GetBufferPointer());
		}
		else {
			throw "FAILED shader load error: unknown error";
		}
	}
	else if (!shader_blob_) {
		throw "FAILED shader load error: shaderBlob is nullptr";
	}
}