#pragma once

#include "d3dx12.h"
#include <string>
using namespace Microsoft::WRL;

/// <summary>
/// シェーダーのコンパイル用クラス
/// </summary>
class Shader
{
    friend class ShaderFactory;
public:
    /// <summary>
    /// シェーダーの設定
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

        //シェーダーモデルの設定
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
    //シェーダーのロード
    void load(ShaderConf conf);

public://取得系
    //シェーダーバイナリの取得
    ID3D10Blob* getShaderBlob() const { return shader_blob_.Get(); }

private:
    ComPtr<ID3D10Blob> shader_blob_;  //シェーダーバイナリ
};
