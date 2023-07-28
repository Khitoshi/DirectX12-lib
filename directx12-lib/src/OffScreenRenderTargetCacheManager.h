#pragma once
#include "OffScreenRenderTargetFactory.h"

#include "Hashes.h"
#include <vector>
#include <stdexcept>

/// <summary>
/// �I�t�X�N���[�������_�[�^�[�Q�b�g�̃L���b�V���Ǘ��V�X�e��
/// </summary>
class OffScreenRenderTargetCacheManager
{
public:
    struct OffScreenRenderTargetCacheKeyHasher
    {
        std::size_t operator()(const OffScreenRenderTarget::OffScreenRenderTargetConf& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.resource_desc.Dimension);
            hash_combine(seed, k.resource_desc.Alignment);
            hash_combine(seed, k.resource_desc.Width);
            hash_combine(seed, k.resource_desc.Height);
            hash_combine(seed, k.resource_desc.DepthOrArraySize);
            hash_combine(seed, k.resource_desc.MipLevels);
            hash_combine(seed, k.resource_desc.Format);
            hash_combine(seed, k.resource_desc.SampleDesc);
            hash_combine(seed, k.resource_desc.Layout);
            hash_combine(seed, k.resource_desc.Flags);

            hash_combine(seed, k.descriptor_heap_desc.NumDescriptors);
            hash_combine(seed, k.descriptor_heap_desc.Type);

            return seed;
        }
    };
private:
    OffScreenRenderTargetCacheManager() {};
    ~OffScreenRenderTargetCacheManager() {};

public:
    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns></returns>
    static OffScreenRenderTargetCacheManager& getInstance()
    {
        static OffScreenRenderTargetCacheManager instance;
        return instance;
    }

    /// <summary>
    /// �擾or����
    /// </summary>
    /// <param name="device"></param>
    /// <param name="conf"></param>
    /// <returns></returns>
    std::shared_ptr<OffScreenRenderTarget> getOrCreate(ID3D12Device* device, const OffScreenRenderTarget::OffScreenRenderTargetConf& conf) {
        //�L���b�V���ɂ��邩�m�F
        auto it = this->offScreenRenderTargetMapCache.find(conf);
        //�L���b�V���ɂ���Ȃ炻���Ԃ�
        if (it != this->offScreenRenderTargetMapCache.end()) {
            return it->second;
        }

        //����
        create(device, conf);
        it = this->offScreenRenderTargetMapCache.find(conf);
        if (it == this->offScreenRenderTargetMapCache.end()) {
            throw std::runtime_error("OffScreenRenderTargetCacheManager::getOrCreate() OffScreenRenderTarget not found");
        }

        return it->second;
    }
    std::shared_ptr<OffScreenRenderTarget> getOrCreate(ID3D12Device* device) {
        //�L���b�V���ɂ��邩�m�F
        auto it = this->offScreenRenderTargetMapCache.find(this->conf);
        //�L���b�V���ɂ���Ȃ炻���Ԃ�
        if (it != this->offScreenRenderTargetMapCache.end()) {
            return it->second;
        }

        //����
        create(device, this->conf);
        it = this->offScreenRenderTargetMapCache.find(this->conf);
        if (it == this->offScreenRenderTargetMapCache.end()) {
            throw std::runtime_error("OffScreenRenderTargetCacheManager::getOrCreate() OffScreenRenderTarget not found");
        }

        return it->second;
    }


    std::vector<OffScreenRenderTarget*> getRenderTargetList()
    {
        return this->offScreenRenderTargetList;
    }
    OffScreenRenderTarget* getRenderTarget()
    {
        return this->offScreenRenderTargetList.back();
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g���X�g�̒ǉ�
    /// </summary>
    /// <param name="ort"></param>
    void addRenderTargetList(OffScreenRenderTarget* ort)
    {
        offScreenRenderTargetList.push_back(ort);
    }

    /// <summary>
    /// �����_�[�^�[�Q�b�g���X�g���N���A
    /// </summary>
    void clearRenderTargetList()
    {
        offScreenRenderTargetList.clear();
    }

private:
    /// <summary>
    /// �I�t�X�N���[�������_�[�^�[�Q�b�g����
    /// </summary>
    /// <param name="device"></param>
    /// <param name="conf"></param>
    void create(ID3D12Device* device, const OffScreenRenderTarget::OffScreenRenderTargetConf& conf)
    {
        //�L���b�V���ɂȂ��Ȃ�쐬���ĕԂ�
        std::shared_ptr<OffScreenRenderTarget> offScreenRenderTarget = OffScreenRenderTargetFactory::create(conf, device);
        this->offScreenRenderTargetMapCache[conf] = offScreenRenderTarget;
    }

public:
    /// <summary>
    /// �[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�[�n���h���擾
    /// </summary>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilViewHandle() const {
        return this->depthStencilViewHandle;
    }

    /// <summary>
    /// �I�t�X�N���[�������_�[�^�[�Q�b�g�̃f�t�H���g�̐ݒ�擾
    /// </summary>
    /// <returns></returns>
    OffScreenRenderTarget::OffScreenRenderTargetConf getConf() const {
        return this->conf;
    }

    /// <summary>
    /// �r���[�|�[�g�擾
    /// </summary>
    /// <returns></returns>
    D3D12_VIEWPORT getViewport() const {
        return this->viewport;
    }

public:
    /// <summary>
    /// �[�x�X�e���V���Ɏg�p����f�B�X�N���v�^�[�n���h���ݒ�
    /// </summary>
    /// <param name="depthStencilViewHandle"></param>
    void setDepthStencilViewHandle(const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle) {
        this->depthStencilViewHandle = depthStencilViewHandle;
    }

    /// <summary>
    /// �r���[�|�[�g�ݒ�
    /// </summary>
    /// <param name="viewport"></param>
    void setViewport(const D3D12_VIEWPORT viewport) {
        this->viewport = viewport;
    }

    void setConf(const OffScreenRenderTarget::OffScreenRenderTargetConf& conf) {
        this->conf = conf;
    }

private:
    OffScreenRenderTarget::OffScreenRenderTargetConf conf_;         //�I�t�X�N���[�������_�[�^�[�Q�b�g�̃f�t�H���g�̐ݒ�
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle;             //�[�x�X�e���V���r���[�n���h��
    D3D12_VIEWPORT viewport;                                        //�r���[�|�[�g
    //�L���b�V��
    std::unordered_map<OffScreenRenderTarget::OffScreenRenderTargetConf, std::shared_ptr<OffScreenRenderTarget>, OffScreenRenderTargetCacheKeyHasher, std::equal_to<OffScreenRenderTarget::OffScreenRenderTargetConf>> offScreenRenderTargetMapCache;
    std::vector<OffScreenRenderTarget*> offScreenRenderTargetList;  //�����_�[�^�[�Q�b�g���X�g

};
