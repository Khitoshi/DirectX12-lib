#pragma once

#include "d3dx12.h"

using namespace Microsoft::WRL;

struct ShaderConf
{
    const char* filePath;
    const char* entryFuncName;
};

/// <summary>
/// �V�F�[�_�[�̃R���p�C���p�N���X
/// </summary>
class Shader
{
public:
    Shader() :shaderBlob() {};
    ~Shader() {};

    //���_�V�F�[�_�[�̃��[�h
    void LoadVS(ShaderConf conf);
    //�s�N�Z���V�F�[�_�[�̃��[�h
    void LoadPS(ShaderConf conf);

private:
    //�V�F�[�_�[�̃��[�h
    void load(ShaderConf conf, const char* shaderModel);

public:
    //�V�F�[�_�[�o�C�i���̎擾
    ID3D10Blob* getShaderBlob() const { return shaderBlob.Get(); }

private:
    ComPtr<ID3D10Blob> shaderBlob;  //�V�F�[�_�[�o�C�i��
};
