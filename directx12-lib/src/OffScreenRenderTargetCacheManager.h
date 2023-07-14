#pragma once
#include "OffScreenRenderTargetHashes.h"
#include "OffScreenRenderTarget.h"
#include <stdexcept>
/// <summary>
/// オフスクリーンレンダーターゲットのキャッシュ管理システム
/// </summary>
class OffScreenRenderTargetCacheManager
{
public:
    struct OffScreenRenderTargetCacheKeyHasher
    {
        std::size_t operator()(const OffScreenRenderTarget::OffScreenRenderTargetConf& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.resourceDesc.Dimension);
            hash_combine(seed, k.resourceDesc.Alignment);
            hash_combine(seed, k.resourceDesc.Width);
            hash_combine(seed, k.resourceDesc.Height);
            hash_combine(seed, k.resourceDesc.DepthOrArraySize);
            hash_combine(seed, k.resourceDesc.MipLevels);
            hash_combine(seed, k.resourceDesc.Format);
            hash_combine(seed, k.resourceDesc.SampleDesc.Count);
            hash_combine(seed, k.resourceDesc.SampleDesc.Quality);
            hash_combine(seed, k.resourceDesc.Layout);
            hash_combine(seed, k.resourceDesc.Flags);

            return seed;
        }
    };
private:
    OffScreenRenderTargetCacheManager() {};
    ~OffScreenRenderTargetCacheManager() {};
public:
    /// <summary>
    /// シングルトンなインスタンスを取得
    /// </summary>
    /// <returns></returns>
    static OffScreenRenderTargetCacheManager& getInstance()
    {
        static OffScreenRenderTargetCacheManager instance;
        return instance;
    }

    /// <summary>
    /// 取得or生成
    /// </summary>
    /// <param name="device"></param>
    /// <param name="conf"></param>
    /// <returns></returns>
    std::shared_ptr<OffScreenRenderTarget> getOrCreate(ID3D12Device* device, const OffScreenRenderTarget::OffScreenRenderTargetConf& conf) {
        auto it = this->offScreenRenderTargetMapCache.find(conf);
        //キャッシュにあるならそれを返す
        if (it != this->offScreenRenderTargetMapCache.end()) {
            return it->second;
        }

        //生成
        create(device, conf);
        it = this->offScreenRenderTargetMapCache.find(conf);
        if (it == this->offScreenRenderTargetMapCache.end()) {
            throw std::runtime_error("OffScreenRenderTargetCacheManager::getOrCreate() OffScreenRenderTarget not found");
        }

        return it->second;
    }

private:
    /// <summary>
    /// オフスクリーンレンダーターゲット生成
    /// </summary>
    /// <param name="device"></param>
    /// <param name="conf"></param>
    void create(ID3D12Device* device, const OffScreenRenderTarget::OffScreenRenderTargetConf& conf)
    {
        //キャッシュにないなら作成して返す
        std::shared_ptr<OffScreenRenderTarget> offScreenRenderTarget = std::make_shared<OffScreenRenderTarget>(conf);
        offScreenRenderTarget->init(device);
        this->offScreenRenderTargetMapCache[conf] = offScreenRenderTarget;
    }

public:
    /// <summary>
    /// 深度ステンシルに使用するディスクリプターハンドル設定
    /// </summary>
    /// <param name="depthStencilViewHandle"></param>
    void setDepthStencilViewHandle(const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle) {
		this->depthStencilViewHandle = depthStencilViewHandle;
	}

    /// <summary>
    /// ビューポート設定
    /// </summary>
    /// <param name="viewport"></param>
    void setViewport(const D3D12_VIEWPORT viewport) {
        this->viewport = viewport;
    }

private:
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle; //深度ステンシルビューハンドル
    D3D12_VIEWPORT viewport;                            //ビューポート
    std::unordered_map<OffScreenRenderTarget::OffScreenRenderTargetConf, std::shared_ptr<OffScreenRenderTarget>, OffScreenRenderTargetCacheKeyHasher, std::equal_to<OffScreenRenderTarget::OffScreenRenderTargetConf>> offScreenRenderTargetMapCache;
};
