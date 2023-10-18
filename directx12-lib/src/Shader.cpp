#include "Shader.h"
#include <d3dcompiler.h>
#include <stdexcept>
#include <stierr.h>
#include <sstream>
#include <fstream>
#include <atlbase.h>

/// <summary>
/// シェーダーの読み込み
/// </summary>
/// <param name="filePath">シェーダーファイルパス</param>
/// <param name="entryFuncName">シェーダーエントリー関数名</param>
/// <param name="shaderModel">シェーダーバージョン</param>
/// <returns>
/// 正常にコンパイルされたシェーダー
/// </returns>
void Shader::load(ShaderConf conf)
{
#ifdef _DEBUG
	//シェーダーのデバッグを有効にする
	UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compile_flags = 0;
#endif

	//ファイルパスの変換
	wchar_t wfxFilePath[256] = { L"" };
	//mbstowcs(wfxFilePath, filePath, 256);
	mbstowcs_s(nullptr, wfxFilePath, conf.file_path.c_str(), 256);

	//シェーダーのコンパイル
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