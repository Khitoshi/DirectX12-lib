#pragma once
#include "OffScreenRenderTargetFactory.h"

#include <vector>
#include <stdexcept>

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g�̃L���b�V���Ǘ��V�X�e��
/// </summary>
class OffScreenRenderTargetCacheManager
{
private:
    OffScreenRenderTargetCacheManager() :
        off_screen_Render_target_list_()
    {};
    ~OffScreenRenderTargetCacheManager() {};

public:
    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns>�V���O���g���ȃC���X�^���X</returns>
    static OffScreenRenderTargetCacheManager& getInstance()
    {
        static OffScreenRenderTargetCacheManager instance;
        return instance;
    }

    /// <summary>
    /// �`��\��̃����_�[�^�[�Q�b�g���X�g���擾
    /// </summary>
    /// <returns>�����_�[�^�[�Q�b�g���X�g</returns>
    std::vector<OffScreenRenderTarget*> getRenderTargetList() const
    {
        return this->off_screen_Render_target_list_;
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g���X�g�̒ǉ�
    /// ��납��ǉ������
    /// </summary>
    /// <param name="ort">�I�t�X�N���[�������_�[�^�[�Q�b�g</param>
    void addRenderTargetList(OffScreenRenderTarget* ort)
    {
        this->off_screen_Render_target_list_.push_back(ort);
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g���X�g���N���A
    /// </summary>
    void clearRenderTargetList()
    {
        this->off_screen_Render_target_list_.clear();
    }

private:
    std::vector<OffScreenRenderTarget*> off_screen_Render_target_list_;  //�����_�[�^�[�Q�b�g���X�g
};
