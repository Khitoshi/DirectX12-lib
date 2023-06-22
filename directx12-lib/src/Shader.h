#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;


    class Shader
    {
    public:
        Shader() :shaderBlob() {};
        ~Shader() {};



        //頂点シェーダーのロード
        void LoadVS(const char* filePath, const char* entryFuncName);
        //ピクセルシェーダーのロード
        void LoadPS(const char* filePath, const char* entryFuncName);
    private:
        //シェーダーのロード
        void load(const char* filePath, const char* entryFuncName, const char* shaderModel);

    public:
	    //シェーダーバイナリの取得
	    ID3D10Blob* getShaderBlob() const { return shaderBlob.Get(); }
    private:
        ComPtr<ID3D10Blob> shaderBlob;//シェーダーバイナリ
    };

