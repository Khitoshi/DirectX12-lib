#pragma once
#include "OffScreenRenderTargetFactory.h"

#include "Hashes.h"
#include <vector>
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
        //キャッシュにあるか確認
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
    std::shared_ptr<OffScreenRenderTarget> getOrCreate(ID3D12Device* device) {
        //キャッシュにあるか確認
        auto it = this->offScreenRenderTargetMapCache.find(this->conf);
        //キャッシュにあるならそれを返す
        if (it != this->offScreenRenderTargetMapCache.end()) {
            return it->second;
        }

        //生成
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
    /// レンダーターゲットリストの追加
    /// </summary>
    /// <param name="ort"></param>
    void addRenderTargetList(OffScreenRenderTarget* ort)
    {
        offScreenRenderTargetList.push_back(ort);
    }

    /// <summary>
    /// レンダーターゲットリストをクリア
    /// </summary>
    void clearRenderTargetList()
    {
        offScreenRenderTargetList.clear();
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
        std::shared_ptr<OffScreenRenderTarget> offScreenRenderTarget = OffScreenRenderTargetFactory::create(conf, device);
        this->offScreenRenderTargetMapCache[conf] = offScreenRenderTarget;
    }

public:
    /// <summary>
    /// 深度ステンシルバッファのディスクリプターハンドル取得
    /// </summary>
    /// <returns></returns>
    D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilViewHandle() const {
        return this->depthStencilViewHandle;
    }

    /// <summary>
    /// オフスクリーンレンダーターゲットのデフォルトの設定取得
    /// </summary>
    /// <returns></returns>
    OffScreenRenderTarget::OffScreenRenderTargetConf getConf() const {
        return this->conf;
    }

    /// <summary>
    /// ビューポート取得
    /// </summary>
    /// <returns></returns>
    D3D12_VIEWPORT getViewport() const {
        return this->viewport;
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

    void setConf(const OffScreenRenderTarget::OffScreenRenderTargetConf& conf) {
        this->conf = conf;
    }

private:
    OffScreenRenderTarget::OffScreenRenderTargetConf conf_;         //オフスクリーンレンダーターゲットのデフォルトの設定
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle;             //深度ステンシルビューハンドル
    D3D12_VIEWPORT viewport;                                        //ビューポート
    //キャッシュ
    std::unordered_map<OffScreenRenderTarget::OffScreenRenderTargetConf, std::shared_ptr<OffScreenRenderTarget>, OffScreenRenderTargetCacheKeyHasher, std::equal_to<OffScreenRenderTarget::OffScreenRenderTargetConf>> offScreenRenderTargetMapCache;
    std::vector<OffScreenRenderTarget*> offScreenRenderTargetList;  //レンダーターゲットリスト

};
