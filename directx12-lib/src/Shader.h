#pragma once

#include "d3dx12.h"
#include <string>
using namespace Microsoft::WRL;

struct ShaderConf
{
    std::string filePath;
    std::string entryFuncName;

    const enum class ShaderModelType
    {
        Pixel = 0,
        Vertex,
        NumTypes
    };
    const std::string ShaderModelNames[(int)ShaderModelType::NumTypes] = {
        "ps_5_0",
        "vs_5_0",
    };

    //シェーダーモデルの設定
    ShaderModelType currentShaderModelType;

    bool operator==(const ShaderConf& other) const
    {
        return filePath == other.filePath &&
            entryFuncName == other.entryFuncName &&
            (int)currentShaderModelType == (int)other.currentShaderModelType;
    }
};

struct ShaderConfHash
{
    size_t operator()(const ShaderConf& shaderConf) const
    {
        std::hash<std::string> stringHasher;
        size_t filePathHash = stringHasher(shaderConf.filePath);
        size_t entryFuncNameHash = stringHasher(shaderConf.entryFuncName);
        size_t shaderModelTypeHash = std::hash<int>()(static_cast<int>(shaderConf.currentShaderModelType));

        // Combined hash of all values
        return filePathHash ^ (entryFuncNameHash << 1) ^ (shaderModelTypeHash << 2);
    }
};

/// <summary>
/// シェーダーのコンパイル用クラス
/// </summary>
class Shader
{
public:
    Shader() :shaderBlob() {};
    ~Shader() {};

    //シェーダーのロード
    void load(ShaderConf conf);

public:
    //シェーダーバイナリの取得
    ID3D10Blob* getShaderBlob() const { return shaderBlob.Get(); }

private:
    ComPtr<ID3D10Blob> shaderBlob;  //シェーダーバイナリ
};

/// <summary>
/// 最低限のシェーダーのペア
/// </summary>
struct BasicShaderPair {
    std::shared_ptr<Shader> pixelShader;
    std::shared_ptr<Shader> vertexShader;
};
