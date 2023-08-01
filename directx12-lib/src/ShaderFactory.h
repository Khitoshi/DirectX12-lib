#pragma once
#include "Shader.h"
#include <memory>

/// <summary>
/// シェーダーのファクトリクラス
/// </summary>
class ShaderFactory
{
public:
    /// <summary>
    /// シェーダーを生成する
    /// </summary>
    /// <param name="conf">生成に関する設定</param>
    /// <returns>
    /// 生成&ロードに成功した場合はシェーダーのインスタンスを返す
    /// </returns>
    static std::shared_ptr<Shader> create(const Shader::ShaderConf& conf)
    {
        std::shared_ptr<Shader> shader(new Shader());
        shader->load(conf);
        return shader;
    }
};
