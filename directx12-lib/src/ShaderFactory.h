#pragma once
#include "Shader.h"
#include <memory>

/// <summary>
/// �V�F�[�_�[�̃t�@�N�g���N���X
/// </summary>
class ShaderFactory
{
public:
	static std::shared_ptr<Shader> create(const Shader::ShaderConf& conf)
	{
		std::shared_ptr<Shader> shader(new Shader());
		shader->load(conf);
		return shader;
	}
};
