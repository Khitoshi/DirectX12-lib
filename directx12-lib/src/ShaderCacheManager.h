#pragma once
#include "ShaderFactory.h"
#include <map>
#include <unordered_map>

/// <summary>
/// �V�F�[�_�[�̃L���b�V�����Ǘ�����N���X
/// </summary>
class ShaderCacheManager
{
private:
    //�V�F�[�_�[�̐ݒ���L�[�Ƃ��Ĉ������߂̃n�b�V���֐�
    struct ShaderConfHash
    {
        size_t operator()(const Shader::ShaderConf& shaderConf) const
        {
            std::hash<std::string> stringHasher;
            size_t filePathHash = stringHasher(shaderConf.file_path);
            size_t entryFuncNameHash = stringHasher(shaderConf.entry_func_name);
            size_t shaderModelTypeHash = std::hash<int>()(static_cast<int>(shaderConf.current_shader_model_type));

            // Combined hash of all values
            return filePathHash ^ (entryFuncNameHash << 1) ^ (shaderModelTypeHash << 2);
        }
    };

private:
    ShaderCacheManager() {};
    ~ShaderCacheManager() {};

public:

    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾����
    /// </summary>
    /// <returns>
    /// �V���O���g���ȃC���X�^���X
    /// </returns>
    static ShaderCacheManager& getInstance()
    {
        static ShaderCacheManager instance;
        return instance;
    }

    //�擾�������V�F�[�_�[�̐ݒ���w�肵�ăV�F�[�_�[���擾����
    std::shared_ptr<Shader> getOrCreate(const Shader::ShaderConf& conf);

private:
    std::unordered_map<Shader::ShaderConf, std::shared_ptr<Shader>, ShaderConfHash> shader_cache_;  //�V�F�[�_�[�̃L���b�V��
};
