#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;

struct ShaderConf
{
    const char* filePath;
    const char* entryFuncName;
};

/// <summary>
/// シェーダーのコンパイル用クラス
/// </summary>
class Shader
{
public:
    Shader() :shaderBlob() {};
    ~Shader() {};

    //頂点シェーダーのロード
    void LoadVS(ShaderConf conf);
    //ピクセルシェーダーのロード
    void LoadPS(ShaderConf conf);

private:
    //シェーダーのロード
    void load(ShaderConf conf, const char* shaderModel);

public:
    //シェーダーバイナリの取得
    ID3D10Blob* getShaderBlob() const { return shaderBlob.Get(); }

private:
    ComPtr<ID3D10Blob> shaderBlob;  //シェーダーバイナリ
};
