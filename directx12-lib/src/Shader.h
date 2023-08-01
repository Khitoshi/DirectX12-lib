#pragma once

#include "d3dx12.h"
#include <string>
using namespace Microsoft::WRL;

/// <summary>
/// �V�F�[�_�[�̃R���p�C���p�N���X
/// </summary>
class Shader
{
    friend class ShaderFactory;
public:
    /// <summary>
    /// �V�F�[�_�[�̐ݒ�
    /// </summary>
    struct ShaderConf
    {
        std::string file_path;
        std::string entry_func_name;

        const enum class ShaderModelType
        {
            PIXEL = 0,
            VERTEX,
            NUM
        };
        const std::string ShaderModelNames[(int)ShaderModelType::NUM] = {
            "ps_5_0",
            "vs_5_0",
        };

        //�V�F�[�_�[���f���̐ݒ�
        ShaderModelType current_shader_model_type;

        bool operator==(const ShaderConf& other) const
        {
            return file_path == other.file_path &&
                entry_func_name == other.entry_func_name &&
                (int)current_shader_model_type == (int)other.current_shader_model_type;
        }
    };

private:
    Shader() :shader_blob_() {};

public:
    ~Shader() {};

private:
    //�V�F�[�_�[�̃��[�h
    void load(ShaderConf conf);

public://�擾�n
    //�V�F�[�_�[�o�C�i���̎擾
    ID3D10Blob* getShaderBlob() const { return shader_blob_.Get(); }

private:
    ComPtr<ID3D10Blob> shader_blob_;  //�V�F�[�_�[�o�C�i��
};
