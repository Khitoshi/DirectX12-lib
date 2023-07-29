#pragma once
#include "Shader.h"
#include <memory>

/// <summary>
/// �V�F�[�_�[�̃t�@�N�g���N���X
/// </summary>
class ShaderFactory
{
public:
    /// <summary>
    /// �V�F�[�_�[�𐶐�����
    /// </summary>
    /// <param name="conf">�����Ɋւ���ݒ�</param>
    /// <returns>
    /// ����&���[�h�ɐ��������ꍇ�̓V�F�[�_�[�̃C���X�^���X��Ԃ�
    /// </returns>
    static std::shared_ptr<Shader> create(const Shader::ShaderConf& conf)
    {
        std::shared_ptr<Shader> shader(new Shader());
        shader->load(conf);
        return shader;
    }
};
