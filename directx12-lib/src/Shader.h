#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;


    class Shader
    {
    public:
        Shader() :shaderBlob() {};
        ~Shader() {};



        //���_�V�F�[�_�[�̃��[�h
        void LoadVS(const char* filePath, const char* entryFuncName);
        //�s�N�Z���V�F�[�_�[�̃��[�h
        void LoadPS(const char* filePath, const char* entryFuncName);
    private:
        //�V�F�[�_�[�̃��[�h
        void load(const char* filePath, const char* entryFuncName, const char* shaderModel);

    public:
	    //�V�F�[�_�[�o�C�i���̎擾
	    ID3D10Blob* getShaderBlob() const { return shaderBlob.Get(); }
    private:
        ComPtr<ID3D10Blob> shaderBlob;//�V�F�[�_�[�o�C�i��
    };

