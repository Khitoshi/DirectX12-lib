#pragma once

#include "Texture.h"
#include <memory>

/// <summary>
/// テクスチャのファクトリクラス
/// </summary>
class TextureFactory
{
public:
    /// <summary>
    /// テクスチャを作成する
    /// </summary>
    /// <param name="device">GPUデバイス</param>
    /// <param name="texture_file_path">テクスチャファイルのパス</param>
    /// <returns>
    /// 生成&ロードしたテクスチャのインスタンス
    /// </returns>
    static std::shared_ptr<Texture> create(ID3D12Device* device, const char* texture_file_path)
    {
        std::shared_ptr<Texture> texture(new Texture());
        texture->Load(device, texture_file_path);
        return texture;
    }

};
