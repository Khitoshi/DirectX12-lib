#include "Shader.h"
#include <d3dcompiler.h>
#include <stdexcept>
#include <stierr.h>
#include <sstream>
#include <fstream>
#include <atlbase.h>


/// <summary>
/// 頂点シェーダーの読み込み
/// </summary>
/// <param name="filePath">ファイルパス</param>
/// <param name="entryFuncName">シェーダーエントリー関数名</param>
void Shader::LoadVS(const char* filePath, const char* entryFuncName)
{
    this->shaderBlob = load(filePath, entryFuncName, "vs_5_0");
}


/// <summary>
/// ピクセルシェーダーの読み込み
/// </summary>
/// <param name="filePath">ファイルパス</param>
/// <param name="entryFuncName">シェーダーエントリー関数名</param>
void Shader::LoadPS(const char* filePath, const char* entryFuncName)
{
    this->shaderBlob = load(filePath, entryFuncName, "ps_5_0");
}


/// <summary>
/// シェーダーの読み込み
/// </summary>
/// <param name="filePath">シェーダーファイルパス</param>
/// <param name="entryFuncName">シェーダーエントリー関数名</param>
/// <param name="shaderModel">シェーダーバージョン</param>
/// <returns>
/// 正常にコンパイルされたシェーダー
/// </returns>
ComPtr<ID3D10Blob> Shader::load(const char* filePath, const char* entryFuncName, const char* shaderModel)
{
    ID3DBlob* errorBlob;
#ifdef _DEBUG
    //シェーダーのデバッグを有効にする
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    //ファイルパスの変換
    wchar_t wfxFilePath[256] = { L"" };
    //mbstowcs(wfxFilePath, filePath, 256);
    mbstowcs_s(nullptr, wfxFilePath, filePath, 256);

    //シェーダーのコンパイル
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