#pragma once

#include "TextureFactory.h"
class DescriptorHeap;

/// <summary>
/// �e�N�X�`���̃L���b�V�����Ǘ�����N���X
/// </summary>
class TextureCacheManager
{
private:
    TextureCacheManager() {};
    ~TextureCacheManager() {};

public:

    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns>�V���O���g���ȃC���X�^���X</returns>
    static TextureCacheManager& getInstance()
    {
        static TextureCacheManager instance;
        return instance;
    }

    /// <summary>
    /// �L���b�V�����m�F���āA�Ȃ���΍���ĕԂ�
    /// </summary>
    /// <param name="device"></param>
    /// <param name="path"></param>
    /// <returns></returns>
    std::shared_ptr<Texture> getOrCreate(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* path)
    {
        // ���łɂ���Ȃ炻���Ԃ�
        auto it = this->texture_caches_.find(path);
        if (it != this->texture_caches_.end()) {
            return it->second;
        }

        // �Ȃ��Ȃ����ĕԂ�
        auto texture = TextureFactory::create(device, descriptor_heap, path);
        this->texture_caches_[path] = texture;
        return texture;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> texture_caches_;  // �e�N�X�`���̃L���b�V��
};
