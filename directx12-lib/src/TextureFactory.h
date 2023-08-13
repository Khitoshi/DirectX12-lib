#pragma once

#include "Texture.h"
#include <memory>

/// <summary>
/// �e�N�X�`���̃t�@�N�g���N���X
/// </summary>
class TextureFactory
{
public:
    /// <summary>
    /// �e�N�X�`�����쐬����
    /// </summary>
    /// <param name="device">GPU�f�o�C�X</param>
    /// <param name="texture_file_path">�e�N�X�`���t�@�C���̃p�X</param>
    /// <returns>
    /// ����&���[�h�����e�N�X�`���̃C���X�^���X
    /// </returns>
    static std::shared_ptr<Texture> create(ID3D12Device* device, const char* texture_file_path)
    {
        std::shared_ptr<Texture> texture(new Texture());
        texture->Load(device, texture_file_path);
        return texture;
    }

};
